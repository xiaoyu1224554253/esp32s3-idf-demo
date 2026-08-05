# 灵镜 AI 音响 — 音乐播放器项目

本项目基于 ESP32-S3 + ESP-IDF v6.0.2 开发，目标是在 LCDWiki ES3C28P 开发板上实现一个音乐播放器 UI 及播放功能。

## 相关文档

- [UI 原型设计规格](../superpowers/specs/2026-08-04-music-player-ui-prototype-design.md)
- [需求文档](./requirements.md)
- [硬件资料](./hardware.md)
- [参考资料](./references/)

## UI 原型

- [交互式 HTML 原型](../assets/mockups/music_player_ui_prototype.html)
- [播放页截图](../assets/mockups/ui_player_page.jpg)
- [歌单页截图](../assets/mockups/ui_playlist_page.jpg)
- [搜索页截图](../assets/mockups/ui_search_page.jpg)

> **注意**：当前 UI 原型按 320×240 横屏设计，而 ES3C28P 实际屏幕为 240×320 竖屏。后续需要在 LVGL 移植时进行方向适配或重新设计竖屏布局。

## 目录说明

| 目录 | 用途 |
|------|------|
| `main/` | ESP-IDF 主程序入口 |
| `main/ui/` | UI 实现代码 |
| `components/music_player/` | 音乐播放器核心组件 |
| `assets/` | 图片、字体、UI 素材 |
| `docs/project/` | 项目文档 |
