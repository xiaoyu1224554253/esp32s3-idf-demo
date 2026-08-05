# 阶段 4-6：音频驱动 + SD 卡 + MP3 解码实施计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 完成 ES8311 音频驱动、MicroSD 卡文件系统挂载、MP3 文件枚举与解码播放，形成可独立播放本地音乐的音频引擎。

**Architecture:** `components/bsp` 新增 `bsp_audio.c` / `bsp_sdcard.c`；`components/audio_engine` 负责 MP3 解码和播放控制；播放器核心通过 `audio_engine` 接口控制播放/暂停/音量。

**Tech Stack:** ESP-IDF v6.0.2, esp_codec_dev, ES8311, FATFS/SDMMC, minimp3

---

## 前置依赖

必须已完成 [阶段 0-3 实施计划](./2026-08-05-phase0-bsp-display-touch-plan.md)，即 BSP、LCD、触摸、LVGL 已可用。

---

## 文件结构（本阶段新增）

```
components/
├── bsp/
│   ├── include/
│   │   ├── bsp_audio.h
│   │   └── bsp_sdcard.h
│   ├── src/
│   │   ├── bsp_audio.c
│   │   └── bsp_sdcard.c
│   └── idf_component.yml        # 新增 esp_codec_dev 依赖
└── audio_engine/
    ├── include/
    │   ├── audio_engine.h
    │   └── mp3_decoder.h
    ├── src/
    │   ├── audio_engine.c
    │   └── mp3_decoder.c
    ├── lib/
    │   └── minimp3.h            # 单头文件 MP3 解码器
    └── CMakeLists.txt
```

---

## 阶段 4：ES8311 音频驱动

### Task 4.1: 添加 esp_codec_dev 依赖

**Files:**
- Modify: `components/bsp/idf_component.yml`

- [ ] **Step 1: 创建/修改 idf_component.yml**

```yaml
dependencies:
  espressif/esp_codec_dev: "^1.1.0"
  espressif/esp_lcd_ili9341: "^1.0.0"
```

- [ ] **Step 2: 编译验证**

```bash
idf.py build
```

Expected: 自动下载 `esp_codec_dev` 组件，编译成功。

- [ ] **Step 3: Commit**

```bash
git add components/bsp/idf_component.yml
git commit -m "feat(bsp): add esp_codec_dev dependency"
```

---

### Task 4.2: 实现 ES8311 音频驱动

**Files:**
- Create: `components/bsp/include/bsp_audio.h`
- Create: `components/bsp/src/bsp_audio.c`
- Modify: `components/bsp/CMakeLists.txt`
- Modify: `components/bsp/src/bsp_board.c`

- [ ] **Step 1: 创建 bsp_audio.h**

```c
#ifndef BSP_AUDIO_H
#define BSP_AUDIO_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_AUDIO_SAMPLE_RATE   44100
#define BSP_AUDIO_BITS          16
#define BSP_AUDIO_CHANNELS      2

esp_err_t bsp_audio_init(void);
esp_err_t bsp_audio_set_volume(uint8_t volume);
esp_err_t bsp_audio_start(void);
esp_err_t bsp_audio_stop(void);
int bsp_audio_write(const int16_t *data, int samples);

#ifdef __cplusplus
}
#endif

#endif // BSP_AUDIO_H
```

- [ ] **Step 2: 创建 bsp_audio.c**

```c
#include "bsp_audio.h"
#include "bsp_pins.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "esp_log.h"

static const char *TAG = "bsp_audio";

static i2s_chan_handle_t tx_chan = NULL;
static i2s_chan_handle_t rx_chan = NULL;
static esp_codec_dev_handle_t codec_dev = NULL;
static const audio_codec_data_if_t *data_if = NULL;
static const audio_codec_ctrl_if_t *ctrl_if = NULL;
static const audio_codec_if_t *codec_if = NULL;
static const audio_codec_gpio_if_t *gpio_if = NULL;

extern i2c_master_bus_handle_t bsp_i2c_get_bus(void);

esp_err_t bsp_audio_init(void)
{
    esp_err_t ret = ESP_OK;

    // Create duplex I2S channels
    i2s_chan_config_t chan_cfg = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 4,
        .dma_frame_num = 480,
        .auto_clear_after_cb = true,
        .auto_clear_before_cb = false,
        .intr_priority = 0,
    };
    ret = i2s_new_channel(&chan_cfg, &tx_chan, &rx_chan);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2s channel creation failed: %s", esp_err_to_name(ret));
        return ret;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg = {
            .sample_rate_hz = BSP_AUDIO_SAMPLE_RATE,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256,
        },
        .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
            .slot_mode = I2S_SLOT_MODE_STEREO,
            .slot_mask = I2S_STD_SLOT_BOTH,
            .ws_width = I2S_DATA_BIT_WIDTH_16BIT,
            .ws_pol = false,
            .bit_shift = true,
        },
        .gpio_cfg = {
            .mclk = BSP_AUDIO_I2S_MCLK_GPIO,
            .bclk = BSP_AUDIO_I2S_BCLK_GPIO,
            .ws = BSP_AUDIO_I2S_WS_GPIO,
            .dout = BSP_AUDIO_I2S_DOUT_GPIO,
            .din = BSP_AUDIO_I2S_DIN_GPIO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    ret = i2s_channel_init_std_mode(tx_chan, &std_cfg);
    if (ret != ESP_OK) return ret;
    ret = i2s_channel_init_std_mode(rx_chan, &std_cfg);
    if (ret != ESP_OK) return ret;
    ret = i2s_channel_enable(tx_chan);
    if (ret != ESP_OK) return ret;
    ret = i2s_channel_enable(rx_chan);
    if (ret != ESP_OK) return ret;

    // Create codec interfaces
    audio_codec_i2s_cfg_t i2s_cfg = {
        .port = I2S_NUM_0,
        .rx_handle = rx_chan,
        .tx_handle = tx_chan,
    };
    data_if = audio_codec_new_i2s_data(&i2s_cfg);
    if (data_if == NULL) {
        return ESP_FAIL;
    }

    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = BSP_AUDIO_I2C_NUM,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = bsp_i2c_get_bus(),
    };
    ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    if (ctrl_if == NULL) {
        return ESP_FAIL;
    }

    gpio_if = audio_codec_new_gpio();
    if (gpio_if == NULL) {
        return ESP_FAIL;
    }

    es8311_codec_cfg_t es8311_cfg = {
        .ctrl_if = ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH,
        .pa_pin = BSP_AUDIO_PA_GPIO,
        .use_mclk = true,
        .hw_gain = {
            .pa_voltage = 5.0,
            .codec_dac_voltage = 3.3,
        },
        .pa_reverted = true,
    };
    codec_if = es8311_codec_new(&es8311_cfg);
    if (codec_if == NULL) {
        ESP_LOGE(TAG, "es8311 codec creation failed");
        return ESP_FAIL;
    }

    esp_codec_dev_cfg_t dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_OUT,
        .codec_if = codec_if,
        .data_if = data_if,
    };
    codec_dev = esp_codec_dev_new(&dev_cfg);
    if (codec_dev == NULL) {
        return ESP_FAIL;
    }

    esp_codec_dev_sample_info_t fs = {
        .bits_per_sample = BSP_AUDIO_BITS,
        .channel = BSP_AUDIO_CHANNELS,
        .channel_mask = 0,
        .sample_rate = BSP_AUDIO_SAMPLE_RATE,
        .mclk_multiple = 0,
    };
    ret = esp_codec_dev_open(codec_dev, &fs);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "codec dev open failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "audio initialized");
    return ESP_OK;
}

esp_err_t bsp_audio_set_volume(uint8_t volume)
{
    if (volume > 100) volume = 100;
    if (codec_dev == NULL) return ESP_ERR_INVALID_STATE;
    return esp_codec_dev_set_out_vol(codec_dev, volume);
}

esp_err_t bsp_audio_start(void)
{
    if (codec_dev == NULL) return ESP_ERR_INVALID_STATE;
    return esp_codec_dev_set_out_mute(codec_dev, false);
}

esp_err_t bsp_audio_stop(void)
{
    if (codec_dev == NULL) return ESP_ERR_INVALID_STATE;
    return esp_codec_dev_set_out_mute(codec_dev, true);
}

int bsp_audio_write(const int16_t *data, int samples)
{
    if (codec_dev == NULL || data == NULL) return 0;
    esp_err_t ret = esp_codec_dev_write(codec_dev, (void *)data, samples * sizeof(int16_t));
    return (ret == ESP_OK) ? samples : 0;
}
```

- [ ] **Step 3: 修改 bsp_board.c 添加 I2C 总线共享访问**

在 `bsp_board.c` 中添加全局 I2C 总线句柄和访问函数：

```c
static i2c_master_bus_handle_t i2c_bus = NULL;

i2c_master_bus_handle_t bsp_i2c_get_bus(void)
{
    return i2c_bus;
}
```

修改 `bsp_board_init` 中触摸初始化前创建 I2C 总线（后续触摸驱动改为使用此总线）：

```c
    // Initialize shared I2C bus
    i2c_master_bus_config_t i2c_bus_cfg = {
        .i2c_port = BSP_TOUCH_I2C_NUM,
        .sda_io_num = BSP_TOUCH_I2C_SDA_GPIO,
        .scl_io_num = BSP_TOUCH_I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = 1,
        },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus));

    ret = bsp_backlight_init();
    ...
    ret = bsp_audio_init();
    ...
```

- [ ] **Step 4: 修改 bsp_touch.c 使用共享 I2C 总线**

替换 `bsp_touch_init` 中的 `i2c_new_master_bus` 调用，改为使用 `bsp_i2c_get_bus()`：

```c
    i2c_bus = bsp_i2c_get_bus();
    if (i2c_bus == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
```

并移除局部 `i2c_bus` 静态变量。

- [ ] **Step 5: 修改 bsp/CMakeLists.txt 添加 esp_codec_dev**

```cmake
idf_component_register(
    SRCS "src/bsp_board.c"
         "src/bsp_lcd.c"
         "src/bsp_touch.c"
         "src/bsp_backlight.c"
         "src/bsp_audio.c"
         "src/bsp_sdcard.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_lcd esp_timer esp_codec_dev fatfs sdmmc
)
```

- [ ] **Step 6: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 7: Commit**

```bash
git add components/bsp/
git commit -m "feat(bsp): add es8311 audio driver"
```

---

### Task 4.3: 音频正弦波测试

**Files:**
- Modify: `main/main.c`

- [ ] **Step 1: 修改 main.c 播放 1kHz 正弦波**

```c
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_audio.h"

static const char *TAG = "MAIN";

#define SINE_SAMPLES 256
static int16_t sine_buffer[SINE_SAMPLES];

void app_main(void)
{
    ESP_LOGI(TAG, "Audio sine wave test");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    bsp_audio_set_volume(50);
    bsp_audio_start();

    // Generate 1kHz sine wave at 44.1kHz sample rate
    for (int i = 0; i < SINE_SAMPLES; i++) {
        sine_buffer[i] = (int16_t)(30000 * sin(2 * M_PI * 1000 * i / BSP_AUDIO_SAMPLE_RATE));
    }

    while (1) {
        bsp_audio_write(sine_buffer, SINE_SAMPLES);
    }
}
```

- [ ] **Step 2: 修改 main/CMakeLists.txt 仅依赖 bsp**

```cmake
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES bsp)
```

- [ ] **Step 3: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 4: Commit**

```bash
git add main/main.c main/CMakeLists.txt
git commit -m "feat(audio): add sine wave test"
```

> **验证方式**：烧录后喇叭应输出 1kHz 正弦波声音。

---

## 阶段 5：SD 卡与文件系统

### Task 5.1: 实现 SD 卡驱动

**Files:**
- Create: `components/bsp/include/bsp_sdcard.h`
- Create: `components/bsp/src/bsp_sdcard.c`

- [ ] **Step 1: 创建 bsp_sdcard.h**

```c
#ifndef BSP_SDCARD_H
#define BSP_SDCARD_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_sdcard_init(void);
const char *bsp_sdcard_get_mount_point(void);
bool bsp_sdcard_is_mounted(void);

#ifdef __cplusplus
}
#endif

#endif // BSP_SDCARD_H
```

- [ ] **Step 2: 创建 bsp_sdcard.c**

```c
#include "bsp_sdcard.h"
#include "bsp_pins.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"

static const char *TAG = "bsp_sdcard";
static const char *mount_point = "/sdcard";
static sdmmc_card_t *card = NULL;
static bool mounted = false;

esp_err_t bsp_sdcard_init(void)
{
    esp_err_t ret = ESP_OK;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_PROBING;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk = GPIO_NUM_38;
    slot_config.cmd = GPIO_NUM_40;
    slot_config.d0 = GPIO_NUM_39;
    slot_config.d1 = GPIO_NUM_41;
    slot_config.d2 = GPIO_NUM_48;
    slot_config.d3 = GPIO_NUM_47;
    slot_config.width = 4;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "sdcard mount failed: %s", esp_err_to_name(ret));
        return ret;
    }

    mounted = true;
    sdmmc_card_print_info(stdout, card);
    ESP_LOGI(TAG, "sdcard mounted at %s", mount_point);
    return ESP_OK;
}

const char *bsp_sdcard_get_mount_point(void)
{
    return mount_point;
}

bool bsp_sdcard_is_mounted(void)
{
    return mounted;
}
```

- [ ] **Step 3: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 4: Commit**

```bash
git add components/bsp/
git commit -m "feat(bsp): add sd card driver"
```

---

### Task 5.2: 测试 SD 卡文件枚举

**Files:**
- Modify: `main/main.c`

- [ ] **Step 1: 修改 main.c 枚举 MP3 文件**

```c
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_sdcard.h"

static const char *TAG = "MAIN";

static bool is_mp3(const char *filename)
{
    size_t len = strlen(filename);
    return len > 4 && strcasecmp(filename + len - 4, ".mp3") == 0;
}

void app_main(void)
{
    ESP_LOGI(TAG, "SD card file enumeration test");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    DIR *dir = opendir(bsp_sdcard_get_mount_point());
    if (dir == NULL) {
        ESP_LOGE(TAG, "failed to open dir");
        return;
    }

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (is_mp3(entry->d_name)) {
            ESP_LOGI(TAG, "MP3: %s", entry->d_name);
            count++;
        }
    }
    closedir(dir);

    ESP_LOGI(TAG, "found %d mp3 files", count);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
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
git commit -m "feat(storage): add sd card mp3 enumeration test"
```

> **验证方式**：插入包含 MP3 文件的 MicroSD 卡，烧录后串口应列出所有 MP3 文件名。

---

## 阶段 6：MP3 解码与播放引擎

### Task 6.1: 集成 minimp3 解码器

**Files:**
- Create: `components/audio_engine/lib/minimp3.h`
- Create: `components/audio_engine/include/mp3_decoder.h`
- Create: `components/audio_engine/src/mp3_decoder.c`
- Create: `components/audio_engine/CMakeLists.txt`

- [ ] **Step 1: 下载 minimp3**

```bash
mkdir -p components/audio_engine/lib
wget -O components/audio_engine/lib/minimp3.h https://raw.githubusercontent.com/lieff/minimp3/master/minimp3.h
wget -O components/audio_engine/lib/minimp3_ex.h https://raw.githubusercontent.com/lieff/minimp3/master/minimp3_ex.h
```

- [ ] **Step 2: 创建 mp3_decoder.h**

```c
#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int sample_rate;
    int channels;
    int bitrate_kbps;
} mp3_info_t;

typedef struct mp3_decoder mp3_decoder_t;

mp3_decoder_t *mp3_decoder_create(void);
void mp3_decoder_destroy(mp3_decoder_t *dec);
bool mp3_decoder_open_file(mp3_decoder_t *dec, const char *path);
void mp3_decoder_close(mp3_decoder_t *dec);
int mp3_decoder_decode_frame(mp3_decoder_t *dec, int16_t *pcm, int pcm_size, mp3_info_t *info);

#ifdef __cplusplus
}
#endif

#endif // MP3_DECODER_H
```

- [ ] **Step 3: 创建 mp3_decoder.c**

```c
#include "mp3_decoder.h"
#include "minimp3.h"
#include "minimp3_ex.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "mp3_decoder";

struct mp3_decoder {
    mp3dec_t mp3d;
    mp3dec_file_info_t info;
    FILE *file;
    uint8_t *file_buffer;
    size_t file_size;
    size_t decode_pos;
};

mp3_decoder_t *mp3_decoder_create(void)
{
    mp3_decoder_t *dec = calloc(1, sizeof(mp3_decoder_t));
    if (dec == NULL) return NULL;
    mp3dec_init(&dec->mp3d);
    return dec;
}

void mp3_decoder_destroy(mp3_decoder_t *dec)
{
    if (dec == NULL) return;
    mp3_decoder_close(dec);
    free(dec);
}

bool mp3_decoder_open_file(mp3_decoder_t *dec, const char *path)
{
    if (dec == NULL || path == NULL) return false;

    mp3_decoder_close(dec);

    dec->file = fopen(path, "rb");
    if (dec->file == NULL) {
        ESP_LOGE(TAG, "failed to open %s", path);
        return false;
    }

    fseek(dec->file, 0, SEEK_END);
    dec->file_size = ftell(dec->file);
    fseek(dec->file, 0, SEEK_SET);

    dec->file_buffer = malloc(dec->file_size);
    if (dec->file_buffer == NULL) {
        fclose(dec->file);
        dec->file = NULL;
        return false;
    }

    size_t read = fread(dec->file_buffer, 1, dec->file_size, dec->file);
    if (read != dec->file_size) {
        ESP_LOGW(TAG, "only read %zu of %zu bytes", read, dec->file_size);
    }

    dec->decode_pos = 0;
    ESP_LOGI(TAG, "opened %s, size %zu", path, dec->file_size);
    return true;
}

void mp3_decoder_close(mp3_decoder_t *dec)
{
    if (dec == NULL) return;
    if (dec->file) {
        fclose(dec->file);
        dec->file = NULL;
    }
    if (dec->file_buffer) {
        free(dec->file_buffer);
        dec->file_buffer = NULL;
    }
    dec->file_size = 0;
    dec->decode_pos = 0;
}

int mp3_decoder_decode_frame(mp3_decoder_t *dec, int16_t *pcm, int pcm_size, mp3_info_t *info)
{
    if (dec == NULL || pcm == NULL) return 0;
    if (dec->file_buffer == NULL || dec->decode_pos >= dec->file_size) return 0;

    mp3dec_frame_info_t frame_info;
    int samples = mp3dec_decode_frame(&dec->mp3d,
                                       dec->file_buffer + dec->decode_pos,
                                       dec->file_size - dec->decode_pos,
                                       pcm, &frame_info);

    if (info != NULL) {
        info->sample_rate = frame_info.hz;
        info->channels = frame_info.channels;
        info->bitrate_kbps = frame_info.bitrate_kbps;
    }

    if (samples > 0) {
        dec->decode_pos += frame_info.frame_bytes;
    } else {
        dec->decode_pos++;
    }

    return samples;
}
```

- [ ] **Step 4: 创建 audio_engine CMakeLists.txt**

```cmake
idf_component_register(
    SRCS "src/audio_engine.c"
         "src/mp3_decoder.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "lib"
    REQUIRES bsp freertos
)
```

- [ ] **Step 5: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 6: Commit**

```bash
git add components/audio_engine/
git commit -m "feat(audio): integrate minimp3 decoder"
```

---

### Task 6.2: 实现音频播放引擎

**Files:**
- Create: `components/audio_engine/include/audio_engine.h`
- Create: `components/audio_engine/src/audio_engine.c`

- [ ] **Step 1: 创建 audio_engine.h**

```c
#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t audio_engine_init(void);
esp_err_t audio_engine_play_file(const char *path);
esp_err_t audio_engine_stop(void);
esp_err_t audio_engine_pause(void);
esp_err_t audio_engine_resume(void);
esp_err_t audio_engine_set_volume(uint8_t volume);
bool audio_engine_is_playing(void);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_ENGINE_H
```

- [ ] **Step 2: 创建 audio_engine.c**

```c
#include "audio_engine.h"
#include "mp3_decoder.h"
#include "bsp_audio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "audio_engine";

#define AUDIO_ENGINE_TASK_STACK_SIZE    8192
#define AUDIO_ENGINE_TASK_PRIORITY      5
#define PCM_BUFFER_SAMPLES              1152 * 2

static TaskHandle_t s_task_handle = NULL;
static mp3_decoder_t *s_decoder = NULL;
static char s_current_path[256] = {0};
static volatile bool s_playing = false;
static volatile bool s_paused = false;
static uint8_t s_volume = 50;

static int16_t s_pcm_buffer[PCM_BUFFER_SAMPLES];

static void audio_engine_task(void *arg)
{
    while (1) {
        if (!s_playing) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (s_paused) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        mp3_info_t info;
        int samples = mp3_decoder_decode_frame(s_decoder, s_pcm_buffer,
                                                PCM_BUFFER_SAMPLES, &info);
        if (samples <= 0) {
            ESP_LOGI(TAG, "playback finished");
            s_playing = false;
            continue;
        }

        // Convert mono to stereo if needed
        if (info.channels == 1) {
            for (int i = samples - 1; i >= 0; i--) {
                s_pcm_buffer[i * 2 + 1] = s_pcm_buffer[i];
                s_pcm_buffer[i * 2] = s_pcm_buffer[i];
            }
            samples *= 2;
        }

        bsp_audio_write(s_pcm_buffer, samples);
    }
}

esp_err_t audio_engine_init(void)
{
    s_decoder = mp3_decoder_create();
    if (s_decoder == NULL) {
        return ESP_ERR_NO_MEM;
    }

    xTaskCreate(audio_engine_task, "audio_engine", AUDIO_ENGINE_TASK_STACK_SIZE,
                NULL, AUDIO_ENGINE_TASK_PRIORITY, &s_task_handle);

    ESP_LOGI(TAG, "audio engine initialized");
    return ESP_OK;
}

esp_err_t audio_engine_play_file(const char *path)
{
    if (path == NULL || s_decoder == NULL) return ESP_ERR_INVALID_STATE;

    s_playing = false;
    vTaskDelay(pdMS_TO_TICKS(50));

    if (!mp3_decoder_open_file(s_decoder, path)) {
        return ESP_FAIL;
    }

    strncpy(s_current_path, path, sizeof(s_current_path) - 1);
    s_current_path[sizeof(s_current_path) - 1] = '\0';

    s_paused = false;
    s_playing = true;

    ESP_LOGI(TAG, "playing %s", path);
    return ESP_OK;
}

esp_err_t audio_engine_stop(void)
{
    s_playing = false;
    return ESP_OK;
}

esp_err_t audio_engine_pause(void)
{
    if (s_playing) {
        s_paused = true;
    }
    return ESP_OK;
}

esp_err_t audio_engine_resume(void)
{
    if (s_playing) {
        s_paused = false;
    }
    return ESP_OK;
}

esp_err_t audio_engine_set_volume(uint8_t volume)
{
    s_volume = volume > 100 ? 100 : volume;
    return bsp_audio_set_volume(s_volume);
}

bool audio_engine_is_playing(void)
{
    return s_playing && !s_paused;
}
```

- [ ] **Step 3: 修改 main.c 测试 MP3 播放**

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_sdcard.h"
#include "audio_engine.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "MP3 playback test");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    ret = audio_engine_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "audio engine init failed");
        return;
    }

    audio_engine_set_volume(50);
    audio_engine_play_file("/sdcard/test.mp3");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

- [ ] **Step 4: 修改 main/CMakeLists.txt 添加 audio_engine 依赖**

```cmake
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES bsp audio_engine)
```

- [ ] **Step 5: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 6: Commit**

```bash
git add components/audio_engine/ main/
git commit -m "feat(audio): add mp3 playback engine"
```

> **阶段 6 交付验证**：在 MicroSD 卡根目录放置 `test.mp3`，烧录后喇叭应播放该 MP3 文件。

---

## 阶段 4-6 完成检查清单

- [ ] ES8311 音频驱动可用，正弦波测试通过
- [ ] MicroSD 卡可挂载，可枚举 MP3 文件
- [ ] minimp3 解码器集成成功
- [ ] 音频引擎可播放 SD 卡中的 MP3 文件
- [ ] 所有代码已提交

---

## 进入下一阶段

阶段 4-6 完成后，开始执行 [阶段 7-9 详细计划：播放器逻辑 + UI + 集成测试](./2026-08-05-phase2-player-ui-plan.md)。
