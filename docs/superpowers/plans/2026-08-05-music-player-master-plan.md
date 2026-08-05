# 灵镜 AI 音响音乐播放器 — 实施主计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在 ESP32-S3 (ES3C28P) 开发板上实现一个基于 LVGL 的横屏音乐播放器，支持 MicroSD 卡 MP3 播放、电容触摸交互。

**Architecture:** 采用分层架构：底层是 BSP（板级支持包）驱动 ILI9341/FT6336G/ES8311/SD 卡；中间层是 LVGL 显示与输入移植；上层是音乐播放器核心逻辑与 UI 页面。组件间通过清晰接口解耦，便于独立测试。

**Tech Stack:** ESP-IDF v6.0.2, LVGL v9, esp_codec_dev, FATFS/SDMMC, minimp3, FreeRTOS

---

## 执行原则

1. **严格按计划执行**：每个阶段完成后才能进入下一阶段，不得跳过或合并任务。
2. **每阶段必须可编译、可验证**：每个任务结束后运行 `idf.py build`，确保无错误。
3. **每阶段必须 commit**：完成一个阶段后提交一次，提交信息遵循 conventional commits。
4. **不引入未计划功能**：如 AI 语音、网络电台等，必须在需求确认后单独补充计划。

---

## 文件结构总览

```
/workspace/
├── CMakeLists.txt
├── main/
│   ├── main.c
│   ├── CMakeLists.txt
│   └── ui/                      # LVGL UI 页面
│       └── .gitkeep
├── components/
│   ├── bsp/                     # 板级支持包（新增）
│   │   ├── include/
│   │   │   ├── bsp_board.h
│   │   │   ├── bsp_lcd.h
│   │   │   ├── bsp_touch.h
│   │   │   ├── bsp_audio.h
│   │   │   ├── bsp_sdcard.h
│   │   │   └── bsp_backlight.h
│   │   ├── src/
│   │   │   ├── bsp_board.c
│   │   │   ├── bsp_lcd.c
│   │   │   ├── bsp_touch.c
│   │   │   ├── bsp_audio.c
│   │   │   ├── bsp_sdcard.c
│   │   │   └── bsp_backlight.c
│   │   └── CMakeLists.txt
│   ├── lvgl_port/               # LVGL 移植层（新增）
│   │   ├── include/
│   │   │   └── lvgl_port.h
│   │   ├── src/
│   │   │   └── lvgl_port.c
│   │   └── CMakeLists.txt
│   ├── audio_engine/            # 音频播放引擎（新增）
│   │   ├── include/
│   │   │   ├── audio_engine.h
│   │   │   └── mp3_decoder.h
│   │   ├── src/
│   │   │   ├── audio_engine.c
│   │   │   └── mp3_decoder.c
│   │   └── CMakeLists.txt
│   └── music_player/            # 已存在，扩展
│       ├── include/
│       │   └── music_player.h
│       ├── src/
│       │   └── music_player.c
│       └── CMakeLists.txt
├── assets/
│   ├── fonts/
│   └── images/
└── docs/
    └── superpowers/
        └── plans/
            ├── 2026-08-05-music-player-master-plan.md
            ├── 2026-08-05-phase0-bsp-display-touch-plan.md
            ├── 2026-08-05-phase1-audio-sdcard-plan.md
            └── 2026-08-05-phase2-player-ui-plan.md
```

---

## 阶段划分与交付物

| 阶段 | 名称 | 交付物 | 依赖 |
|------|------|--------|------|
| 0 | 项目基础配置 | `components/bsp/` 骨架、`idf_component.yml` 依赖 | 无 |
| 1 | ILI9341 显示驱动 | 屏幕点亮，可填充颜色，背光可控 | 阶段 0 |
| 2 | FT6336G 触摸驱动 | 触摸坐标读取，横屏映射 | 阶段 0 |
| 3 | LVGL 移植与横屏 | LVGL 运行，显示测试页面，触摸可用 | 阶段 1、2 |
| 4 | ES8311 音频驱动 | 音频输出可用，可播放正弦波测试 | 阶段 0 |
| 5 | SD 卡与文件系统 | 可枚举 MicroSD 卡中的 MP3 文件 | 阶段 0 |
| 6 | MP3 解码与播放引擎 | 可解码并播放 MP3 文件 | 阶段 4、5 |
| 7 | 音乐播放器核心逻辑 | 播放列表、播放模式、音量、进度 | 阶段 6 |
| 8 | UI 页面实现 | 播放页、歌单页、电台页、搜索页 | 阶段 3、7 |
| 9 | 系统集成与测试 | 完整音乐播放器，通过测试清单 | 阶段 8 |

---

## 各阶段详细计划

- [阶段 0-3 详细计划：BSP + 显示 + 触摸 + LVGL](./2026-08-05-phase0-bsp-display-touch-plan.md)
- [阶段 4-6 详细计划：音频 + SD 卡 + MP3 解码](./2026-08-05-phase1-audio-sdcard-plan.md)
- [阶段 7-9 详细计划：播放器逻辑 + UI + 集成测试](./2026-08-05-phase2-player-ui-plan.md)

---

## 通用提交规范

每个阶段结束后执行：

```bash
git add -A
git commit -m "feat(<scope>): <description>"
```

scope 建议：
- `bsp`：板级支持包
- `lvgl`：LVGL 移植
- `audio`：音频相关
- `player`：播放器逻辑
- `ui`：UI 页面
- `docs`：文档

---

## 风险与待确认项

1. **MicroSD 卡 SDIO 模式**：ES3C28P 使用 4-bit SDIO，需在 ESP-IDF 中正确配置。
2. **PSRAM 使用**：MP3 解码和 LVGL 缓冲可能需要使用 PSRAM。
3. **MP3 解码器选择**：优先使用 `minimp3` 或 `esp-audio-speech-codec`，需在阶段 6 前确认。
4. **网络功能**：AI 语音、网络电台等未纳入本计划，需单独规划。
