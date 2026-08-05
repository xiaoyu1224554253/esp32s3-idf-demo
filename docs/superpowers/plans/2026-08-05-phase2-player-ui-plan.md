# 阶段 7-9：播放器逻辑 + UI + 集成测试实施计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 实现音乐播放器核心逻辑（播放列表、播放模式、音量、进度），基于 LVGL 完成播放页、歌单页等 UI 页面，最终集成为可交互的完整音乐播放器。

**Architecture:** `components/music_player` 负责业务逻辑和状态管理；`main/ui/` 包含 LVGL 页面代码，通过调用 `music_player` 接口响应用户操作；`main/main.c` 初始化所有模块并启动 UI。

**Tech Stack:** ESP-IDF v6.0.2, LVGL v9, FreeRTOS

---

## 前置依赖

必须已完成 [阶段 0-3 实施计划](./2026-08-05-phase0-bsp-display-touch-plan.md) 和 [阶段 4-6 实施计划](./2026-08-05-phase1-audio-sdcard-plan.md)。

---

## 文件结构（本阶段新增/修改）

```
components/
└── music_player/
    ├── include/
    │   └── music_player.h       # 扩展接口
    ├── src/
    │   └── music_player.c       # 扩展实现
    └── CMakeLists.txt
main/
├── ui/
│   ├── music_player_ui.h
│   ├── page_now_playing.c
│   ├── page_playlist.c
│   └── page_radio.c
├── main.c
└── CMakeLists.txt
```

---

## 阶段 7：音乐播放器核心逻辑

### Task 7.1: 扩展 music_player 组件接口

**Files:**
- Modify: `components/music_player/include/music_player.h`
- Modify: `components/music_player/music_player.c`
- Modify: `components/music_player/CMakeLists.txt`

- [ ] **Step 1: 修改 music_player.h**

```c
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MUSIC_PLAYER_MAX_TRACKS 256
#define MUSIC_PLAYER_MAX_PATH_LEN 256
#define MUSIC_PLAYER_MAX_TITLE_LEN 128
#define MUSIC_PLAYER_MAX_ARTIST_LEN 128

typedef enum {
    PLAY_MODE_SEQUENTIAL,
    PLAY_MODE_RANDOM,
    PLAY_MODE_REPEAT_ONE,
    PLAY_MODE_REPEAT_ALL,
} music_player_play_mode_t;

typedef struct {
    char path[MUSIC_PLAYER_MAX_PATH_LEN];
    char title[MUSIC_PLAYER_MAX_TITLE_LEN];
    char artist[MUSIC_PLAYER_MAX_ARTIST_LEN];
    uint32_t duration_ms;
} music_player_track_t;

typedef struct {
    music_player_track_t tracks[MUSIC_PLAYER_MAX_TRACKS];
    uint32_t track_count;
    int32_t current_index;
    bool is_playing;
    uint8_t volume;
    music_player_play_mode_t play_mode;
    uint32_t progress_ms;
    uint32_t duration_ms;
} music_player_state_t;

void music_player_init(void);

// Playlist management
esp_err_t music_player_scan_sdcard(void);
uint32_t music_player_get_track_count(void);
const music_player_track_t *music_player_get_track(uint32_t index);
const music_player_state_t *music_player_get_state(void);

// Playback control
void music_player_play(uint32_t index);
void music_player_pause(void);
void music_player_resume(void);
void music_player_toggle(void);
void music_player_next(void);
void music_player_prev(void);
void music_player_stop(void);

// Volume and mode
void music_player_set_volume(uint8_t volume);
uint8_t music_player_get_volume(void);
void music_player_set_play_mode(music_player_play_mode_t mode);
music_player_play_mode_t music_player_get_play_mode(void);

// Progress
void music_player_set_progress(uint32_t progress_ms);
uint32_t music_player_get_progress_ms(void);

// Status
bool music_player_is_playing(void);

#ifdef __cplusplus
}
#endif
```

- [ ] **Step 2: 修改 music_player.c**

```c
#include "music_player.h"
#include "audio_engine.h"
#include "bsp_sdcard.h"
#include "esp_log.h"
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

static const char *TAG = "music_player";

static music_player_state_t s_state = {0};

static bool is_mp3(const char *filename)
{
    size_t len = strlen(filename);
    return len > 4 && strcasecmp(filename + len - 4, ".mp3") == 0;
}

static void extract_title(const char *filename, char *out_title, size_t out_len)
{
    strncpy(out_title, filename, out_len - 1);
    out_title[out_len - 1] = '\0';

    size_t len = strlen(out_title);
    if (len > 4 && strcasecmp(out_title + len - 4, ".mp3") == 0) {
        out_title[len - 4] = '\0';
    }
}

void music_player_init(void)
{
    memset(&s_state, 0, sizeof(s_state));
    s_state.volume = 50;
    s_state.play_mode = PLAY_MODE_SEQUENTIAL;
    s_state.current_index = -1;

    audio_engine_init();
    audio_engine_set_volume(s_state.volume);

    ESP_LOGI(TAG, "music player initialized");
}

esp_err_t music_player_scan_sdcard(void)
{
    s_state.track_count = 0;

    if (!bsp_sdcard_is_mounted()) {
        ESP_LOGE(TAG, "sdcard not mounted");
        return ESP_ERR_INVALID_STATE;
    }

    DIR *dir = opendir(bsp_sdcard_get_mount_point());
    if (dir == NULL) {
        ESP_LOGE(TAG, "failed to open sdcard dir");
        return ESP_FAIL;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!is_mp3(entry->d_name)) continue;
        if (s_state.track_count >= MUSIC_PLAYER_MAX_TRACKS) break;

        music_player_track_t *track = &s_state.tracks[s_state.track_count];
        snprintf(track->path, sizeof(track->path), "%s/%s",
                 bsp_sdcard_get_mount_point(), entry->d_name);
        extract_title(entry->d_name, track->title, sizeof(track->title));
        strncpy(track->artist, "Unknown Artist", sizeof(track->artist) - 1);
        track->duration_ms = 0;

        s_state.track_count++;
    }
    closedir(dir);

    ESP_LOGI(TAG, "scanned %lu tracks", s_state.track_count);
    return ESP_OK;
}

uint32_t music_player_get_track_count(void)
{
    return s_state.track_count;
}

const music_player_track_t *music_player_get_track(uint32_t index)
{
    if (index >= s_state.track_count) return NULL;
    return &s_state.tracks[index];
}

const music_player_state_t *music_player_get_state(void)
{
    return &s_state;
}

void music_player_play(uint32_t index)
{
    if (index >= s_state.track_count) return;

    s_state.current_index = index;
    s_state.progress_ms = 0;
    s_state.duration_ms = s_state.tracks[index].duration_ms;

    audio_engine_play_file(s_state.tracks[index].path);
    s_state.is_playing = true;

    ESP_LOGI(TAG, "play track %lu: %s", index, s_state.tracks[index].title);
}

void music_player_pause(void)
{
    audio_engine_pause();
    s_state.is_playing = false;
    ESP_LOGI(TAG, "paused");
}

void music_player_resume(void)
{
    if (s_state.current_index < 0 && s_state.track_count > 0) {
        music_player_play(0);
        return;
    }
    audio_engine_resume();
    s_state.is_playing = true;
    ESP_LOGI(TAG, "resumed");
}

void music_player_toggle(void)
{
    if (s_state.is_playing) {
        music_player_pause();
    } else {
        music_player_resume();
    }
}

void music_player_next(void)
{
    if (s_state.track_count == 0) return;

    int next = 0;
    switch (s_state.play_mode) {
        case PLAY_MODE_RANDOM:
            next = rand() % s_state.track_count;
            break;
        case PLAY_MODE_REPEAT_ONE:
            next = s_state.current_index >= 0 ? s_state.current_index : 0;
            break;
        default:
            next = s_state.current_index + 1;
            if (next >= (int)s_state.track_count) {
                next = 0;
            }
            break;
    }
    music_player_play(next);
}

void music_player_prev(void)
{
    if (s_state.track_count == 0) return;

    int prev = s_state.current_index - 1;
    if (prev < 0) {
        prev = s_state.track_count - 1;
    }
    music_player_play(prev);
}

void music_player_stop(void)
{
    audio_engine_stop();
    s_state.is_playing = false;
    s_state.current_index = -1;
    s_state.progress_ms = 0;
}

void music_player_set_volume(uint8_t volume)
{
    if (volume > 100) volume = 100;
    s_state.volume = volume;
    audio_engine_set_volume(volume);
}

uint8_t music_player_get_volume(void)
{
    return s_state.volume;
}

void music_player_set_play_mode(music_player_play_mode_t mode)
{
    s_state.play_mode = mode;
}

music_player_play_mode_t music_player_get_play_mode(void)
{
    return s_state.play_mode;
}

void music_player_set_progress(uint32_t progress_ms)
{
    s_state.progress_ms = progress_ms;
}

uint32_t music_player_get_progress_ms(void)
{
    return s_state.progress_ms;
}

bool music_player_is_playing(void)
{
    return s_state.is_playing;
}
```

- [ ] **Step 3: 修改 music_player/CMakeLists.txt**

```cmake
idf_component_register(
    SRCS "src/music_player.c"
    INCLUDE_DIRS "include"
    REQUIRES audio_engine bsp
)
```

- [ ] **Step 4: 创建 src 目录并移动文件**

```bash
mkdir -p components/music_player/src
mv components/music_player/music_player.c components/music_player/src/music_player.c
```

- [ ] **Step 5: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 6: Commit**

```bash
git add components/music_player/
git commit -m "feat(player): extend music player logic with playlist and playback modes"
```

---

### Task 7.2: 实现播放进度自动更新

**Files:**
- Modify: `components/music_player/src/music_player.c`
- Modify: `components/music_player/include/music_player.h`

- [ ] **Step 1: 在 music_player.h 添加定时器接口**

```c
void music_player_start_progress_timer(void);
void music_player_stop_progress_timer(void);
```

- [ ] **Step 2: 在 music_player.c 添加进度更新任务**

在文件顶部添加：

```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PROGRESS_UPDATE_INTERVAL_MS 1000
```

添加任务函数：

```c
static void progress_update_task(void *arg)
{
    (void)arg;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(PROGRESS_UPDATE_INTERVAL_MS));
        if (s_state.is_playing && s_state.progress_ms < s_state.duration_ms) {
            s_state.progress_ms += PROGRESS_UPDATE_INTERVAL_MS;
        }
    }
}
```

在 `music_player_init` 末尾添加：

```c
    xTaskCreate(progress_update_task, "progress_update", 2048, NULL, 2, NULL);
```

- [ ] **Step 3: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 4: Commit**

```bash
git add components/music_player/
git commit -m "feat(player): add progress auto update"
```

---

## 阶段 8：UI 页面实现

### Task 8.1: 创建 UI 组件头文件

**Files:**
- Create: `main/ui/music_player_ui.h`
- Create: `main/ui/page_now_playing.c`
- Create: `main/ui/page_playlist.c`
- Modify: `main/CMakeLists.txt`

- [ ] **Step 1: 创建 music_player_ui.h**

```c
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(void);
void ui_show_now_playing(void);
void ui_show_playlist(void);
void ui_update_now_playing(void);

#ifdef __cplusplus
}
#endif

#endif // MUSIC_PLAYER_UI_H
```

- [ ] **Step 2: 创建 page_now_playing.c**

```c
#include "music_player_ui.h"
#include "music_player.h"
#include "esp_log.h"

static const char *TAG = "ui_now_playing";

static lv_obj_t *title_label = NULL;
static lv_obj_t *artist_label = NULL;
static lv_obj_t *play_btn = NULL;
static lv_obj_t *play_btn_label = NULL;
static lv_obj_t *progress_bar = NULL;
static lv_obj_t *time_label = NULL;

static void format_time(uint32_t ms, char *buf, size_t len)
{
    uint32_t total_seconds = ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    snprintf(buf, len, "%02lu:%02lu", minutes, seconds);
}

static void play_btn_event_cb(lv_event_t *e)
{
    (void)e;
    music_player_toggle();
    ui_update_now_playing();
}

static void next_btn_event_cb(lv_event_t *e)
{
    (void)e;
    music_player_next();
    ui_update_now_playing();
}

static void prev_btn_event_cb(lv_event_t *e)
{
    (void)e;
    music_player_prev();
    ui_update_now_playing();
}

static void playlist_btn_event_cb(lv_event_t *e)
{
    (void)e;
    ui_show_playlist();
}

void ui_show_now_playing(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x121212), 0);

    // Title
    title_label = lv_label_create(scr);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 30);
    lv_label_set_text(title_label, "No track");

    // Artist
    artist_label = lv_label_create(scr);
    lv_obj_set_style_text_color(artist_label, lv_color_hex(0xaaaaaa), 0);
    lv_obj_align(artist_label, LV_ALIGN_TOP_MID, 0, 60);
    lv_label_set_text(artist_label, "Unknown Artist");

    // Progress bar
    progress_bar = lv_slider_create(scr);
    lv_obj_set_size(progress_bar, 260, 10);
    lv_obj_align(progress_bar, LV_ALIGN_CENTER, 0, 20);
    lv_slider_set_range(progress_bar, 0, 100);

    // Time label
    time_label = lv_label_create(scr);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xaaaaaa), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 45);
    lv_label_set_text(time_label, "00:00 / 00:00");

    // Control buttons
    lv_obj_t *prev_btn = lv_button_create(scr);
    lv_obj_set_size(prev_btn, 60, 40);
    lv_obj_align(prev_btn, LV_ALIGN_BOTTOM_MID, -80, -30);
    lv_obj_add_event_cb(prev_btn, prev_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *prev_label = lv_label_create(prev_btn);
    lv_label_set_text(prev_label, "<<");
    lv_obj_center(prev_label);

    play_btn = lv_button_create(scr);
    lv_obj_set_size(play_btn, 80, 50);
    lv_obj_align(play_btn, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_obj_add_event_cb(play_btn, play_btn_event_cb, LV_EVENT_CLICKED, NULL);
    play_btn_label = lv_label_create(play_btn);
    lv_label_set_text(play_btn_label, "Play");
    lv_obj_center(play_btn_label);

    lv_obj_t *next_btn = lv_button_create(scr);
    lv_obj_set_size(next_btn, 60, 40);
    lv_obj_align(next_btn, LV_ALIGN_BOTTOM_MID, 80, -30);
    lv_obj_add_event_cb(next_btn, next_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, ">>");
    lv_obj_center(next_label);

    // Playlist button
    lv_obj_t *pl_btn = lv_button_create(scr);
    lv_obj_set_size(pl_btn, 100, 36);
    lv_obj_align(pl_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -80);
    lv_obj_add_event_cb(pl_btn, playlist_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *pl_label = lv_label_create(pl_btn);
    lv_label_set_text(pl_label, "List");
    lv_obj_center(pl_label);

    ui_update_now_playing();
}

void ui_update_now_playing(void)
{
    const music_player_state_t *state = music_player_get_state();
    const music_player_track_t *track = NULL;

    if (state->current_index >= 0 && (uint32_t)state->current_index < state->track_count) {
        track = &state->tracks[state->current_index];
    }

    if (title_label) {
        lv_label_set_text(title_label, track ? track->title : "No track");
    }
    if (artist_label) {
        lv_label_set_text(artist_label, track ? track->artist : "Unknown Artist");
    }
    if (play_btn_label) {
        lv_label_set_text(play_btn_label, state->is_playing ? "Pause" : "Play");
    }

    if (progress_bar) {
        int value = 0;
        if (state->duration_ms > 0) {
            value = (int)((state->progress_ms * 100) / state->duration_ms);
        }
        lv_slider_set_value(progress_bar, value, LV_ANIM_OFF);
    }

    if (time_label) {
        char buf[32];
        char total[16];
        format_time(state->progress_ms, buf, sizeof(buf));
        format_time(state->duration_ms, total, sizeof(total));
        strncat(buf, " / ", sizeof(buf) - strlen(buf) - 1);
        strncat(buf, total, sizeof(buf) - strlen(buf) - 1);
        lv_label_set_text(time_label, buf);
    }
}
```

- [ ] **Step 3: 创建 page_playlist.c**

```c
#include "music_player_ui.h"
#include "music_player.h"
#include "esp_log.h"

static const char *TAG = "ui_playlist";

static void track_click_cb(lv_event_t *e)
{
    uint32_t index = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
    music_player_play(index);
    ui_show_now_playing();
}

static void back_btn_event_cb(lv_event_t *e)
{
    (void)e;
    ui_show_now_playing();
}

void ui_show_playlist(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x121212), 0);

    // Title
    lv_obj_t *title = lv_label_create(scr);
    lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_label_set_text(title, "Playlist");

    // Back button
    lv_obj_t *back_btn = lv_button_create(scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    // List
    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, 300, 180);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -10);

    uint32_t count = music_player_get_track_count();
    for (uint32_t i = 0; i < count; i++) {
        const music_player_track_t *track = music_player_get_track(i);
        if (track == NULL) continue;

        lv_obj_t *btn = lv_list_add_button(list, NULL, track->title);
        lv_obj_add_event_cb(btn, track_click_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }
}
```

- [ ] **Step 4: 修改 main/CMakeLists.txt 添加 UI 源文件**

```cmake
idf_component_register(
    SRCS "main.c"
         "ui/page_now_playing.c"
         "ui/page_playlist.c"
    INCLUDE_DIRS "."
                 "ui"
    PRIV_REQUIRES bsp lvgl_port music_player
)
```

- [ ] **Step 5: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 6: Commit**

```bash
git add main/ui/ main/CMakeLists.txt
git commit -m "feat(ui): add now playing and playlist pages"
```

---

### Task 8.2: 实现 UI 自动刷新

**Files:**
- Modify: `main/main.c`
- Modify: `main/ui/music_player_ui.h`
- Modify: `main/ui/page_now_playing.c`

- [ ] **Step 1: 修改 music_player_ui.h 添加刷新函数**

```c
void ui_periodic_update(void);
```

- [ ] **Step 2: 修改 page_now_playing.c 暴露刷新函数**

`ui_update_now_playing` 已经是刷新函数，无需新增。在 `music_player_ui.h` 中已声明。

- [ ] **Step 3: 修改 main.c 启动 UI 并定时刷新**

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "lvgl_port.h"
#include "music_player.h"
#include "music_player_ui.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Music Player starting");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    ret = lvgl_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "LVGL init failed");
        return;
    }

    music_player_init();
    music_player_scan_sdcard();

    ui_init();
    ui_show_now_playing();

    while (1) {
        ui_update_now_playing();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

- [ ] **Step 4: 创建 ui_init 空函数**

在 `page_now_playing.c` 或新建 `ui_init.c` 中：

```c
void ui_init(void)
{
    // Theme setup can be added here
}
```

- [ ] **Step 5: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 6: Commit**

```bash
git add main/
git commit -m "feat(ui): integrate ui with main and auto refresh"
```

---

### Task 8.3: 添加电台页/搜索页框架

**Files:**
- Create: `main/ui/page_radio.c`
- Modify: `main/ui/music_player_ui.h`
- Modify: `main/ui/page_now_playing.c`

- [ ] **Step 1: 创建 page_radio.c**

```c
#include "music_player_ui.h"
#include "esp_log.h"

static const char *TAG = "ui_radio";

static void back_btn_event_cb(lv_event_t *e)
{
    (void)e;
    ui_show_now_playing();
}

void ui_show_radio(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x121212), 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_label_set_text(title, "Radio");

    lv_obj_t *back_btn = lv_button_create(scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    lv_obj_t *info = lv_label_create(scr);
    lv_obj_set_style_text_color(info, lv_color_hex(0xaaaaaa), 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(info, "Network radio not implemented");
}
```

- [ ] **Step 2: 修改 music_player_ui.h 添加 radio 函数**

```c
void ui_show_radio(void);
```

- [ ] **Step 3: 修改 main/CMakeLists.txt 添加 page_radio.c**

```cmake
idf_component_register(
    SRCS "main.c"
         "ui/page_now_playing.c"
         "ui/page_playlist.c"
         "ui/page_radio.c"
    INCLUDE_DIRS "."
                 "ui"
    PRIV_REQUIRES bsp lvgl_port music_player
)
```

- [ ] **Step 4: 在 now playing 页添加 radio 入口按钮**

在 `ui_show_now_playing` 中添加：

```c
    lv_obj_t *radio_btn = lv_button_create(scr);
    lv_obj_set_size(radio_btn, 80, 36);
    lv_obj_align(radio_btn, LV_ALIGN_BOTTOM_LEFT, 10, -80);
    lv_obj_add_event_cb(radio_btn, [](lv_event_t *e) {
        (void)e;
        ui_show_radio();
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_t *radio_label = lv_label_create(radio_btn);
    lv_label_set_text(radio_label, "Radio");
    lv_obj_center(radio_label);
```

> 注意：ESP-IDF C 代码不支持 lambda，需改为普通函数回调。

改为：

```c
static void radio_btn_event_cb(lv_event_t *e)
{
    (void)e;
    ui_show_radio();
}
```

- [ ] **Step 5: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 6: Commit**

```bash
git add main/ui/ main/CMakeLists.txt
git commit -m "feat(ui): add radio page placeholder"
```

---

## 阶段 9：系统集成与测试

### Task 9.1: 最终 main.c 集成

**Files:**
- Modify: `main/main.c`

- [ ] **Step 1: 确认 main.c 内容**

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "lvgl_port.h"
#include "music_player.h"
#include "music_player_ui.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Music Player starting");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    ret = lvgl_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "LVGL init failed");
        return;
    }

    music_player_init();
    music_player_scan_sdcard();

    ui_init();
    ui_show_now_playing();

    while (1) {
        ui_update_now_playing();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

- [ ] **Step 2: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 3: Commit**

```bash
git add main/main.c
git commit -m "feat(main): final integration"
```

---

### Task 9.2: 运行集成测试清单

**Files:**
- Create: `docs/project/testing-checklist.md`

- [ ] **Step 1: 创建测试清单文档**

```markdown
# 灵镜 AI 音响音乐播放器 — 集成测试清单

## 环境准备

- ESP32-S3 ES3C28P 开发板
- MicroSD 卡（FAT32），根目录放置若干 MP3 文件
- USB Type-C 供电/串口

## 测试项

### 1. 启动与显示
- [ ] 上电后屏幕点亮，背光正常
- [ ] 显示深色主题的 "Now Playing" 页面
- [ ] 标题、艺术家、进度条、时间、控制按钮显示正常

### 2. 触摸交互
- [ ] 点击 Play 按钮开始播放
- [ ] 点击 Pause 按钮暂停播放
- [ ] 点击 Next 按钮切换到下一首
- [ ] 点击 Prev 按钮切换到上一首
- [ ] 点击 List 按钮进入歌单页
- [ ] 在歌单页点击歌曲可播放并返回播放页
- [ ] 点击 Radio 按钮进入电台页

### 3. 音频播放
- [ ] 喇叭输出 MP3 音频
- [ ] 音量默认 50%，无明显失真
- [ ] 切歌时音频流畅切换

### 4. 播放模式
- [ ] 顺序播放模式正常工作
- [ ] 随机播放模式可随机切换歌曲
- [ ] 单曲循环模式重复播放同一首
- [ ] 列表循环模式播完最后一首后回到第一首

### 5. 稳定性
- [ ] 连续播放 30 分钟无崩溃
- [ ] 插拔 SD 卡后系统不崩溃（可提示未挂载）

## 已知限制

- 进度拖动尚未实现
- 歌词显示尚未实现
- 封面图片显示尚未实现
- 网络电台为占位页面
```

- [ ] **Step 2: Commit**

```bash
git add docs/project/testing-checklist.md
git commit -m "docs: add integration testing checklist"
```

---

## 阶段 7-9 完成检查清单

- [ ] 音乐播放器核心逻辑完成（播放列表、播放模式、音量、进度）
- [ ] LVGL 播放页、歌单页、电台页实现
- [ ] main.c 完成所有模块集成
- [ ] 项目可编译通过
- [ ] 测试清单已创建
- [ ] 所有代码已提交

---

## 项目完成

完成阶段 7-9 后，灵镜 AI 音响音乐播放器 v1.0 已实现以下功能：

- 基于 ILI9341 的 320×240 横屏显示
- FT6336G 电容触摸交互
- ES8311 音频输出
- MicroSD 卡 MP3 播放
- 播放列表、播放模式、音量控制
- LVGL 播放页、歌单页、电台占位页

后续如需增加 AI 语音、网络电台、歌词显示等功能，需单独制定扩展计划。
