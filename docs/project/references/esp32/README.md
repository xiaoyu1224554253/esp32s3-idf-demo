# ESP32 音乐播放器参考项目分析

本目录收录了 3 个与灵镜 AI 音响高度相关的 ESP32 开源音乐播放器项目，供架构设计、驱动移植、音频实现、UI 实现、网络功能等阶段参考。

## 文件结构

```
docs/project/references/esp32/
├── AppStateV2.5.0round23b-radio-backend-no-external-dep-main/  # 圆屏 ESP32-S3 音乐播放器（PlatformIO）
├── ESP32_LVGL_MusicPlayer-master/                              # LVGL + TFT_eSPI 音乐播放器（PlatformIO）
├── xiaozhi-esp32-music-main/                                   # 小智 AI 音乐固件（ESP-IDF）
├── zips/                                                       # 原始压缩包
└── README.md                                                   # 本文件
```

---

## 项目一：AppStateV2.5.0 — 圆屏 ESP32-S3 音乐播放器

### 定位

最完整的 ESP32-S3 音乐播放器参考工程，基于 **PlatformIO + Arduino 框架**，面向圆屏桌面播放器场景，功能覆盖本地播放、网络电台、NAS/HTTP 歌曲、歌词、封面、NFC、Web 控制。

### 核心能力

| 能力 | 状态 | 说明 |
|------|------|------|
| 本地 MP3/FLAC 播放 | ✅ 完成 | 统一 MP3 解码核心 |
| HTTP 网络电台 | ✅ 完成 | Audio Tools URLStream 收流 |
| NAS/HTTP 网络歌曲 | ✅ 完成 | HTTP MP3 直链播放 |
| 歌词显示 | ✅ 完成 | LRC 解析、当前句高亮 |
| 封面显示 | ✅ 完成 | 内嵌 APIC / FLAC picture / 外部兜底 |
| NFC 绑定播放 | ✅ 完成 | 曲目 / 歌手 / 专辑绑定 |
| Web 控制页 | ✅ 完成 | 状态、切歌、音量、列表、设置 |
| V3 音乐索引 | ✅ 完成 | 二进制索引加速启动 |
| TF 卡热插拔 | ✅ 完成 | 无 CD 脚软件探测方案 |
| 播放模式 | ✅ 完成 | 全部/歌手/专辑 × 顺序/随机 |
| WiFi 配置 | ✅ 完成 | SD 卡 wifi.conf + AP fallback |

### 软件架构

```
App State
├─ Boot
├─ Player
└─ NFC Admin

Player Core
├─ player_state / player_control / player_playlist
├─ player_assets（歌词/封面/时长补齐）
├─ player_snapshot（NVS 快照）
├─ player_source（本地/电台/NAS 来源）
└─ net_music_catalog（NAS offset 索引）

Audio
├─ audio_service（独立任务，命令队列）
├─ audio.cpp（本地文件入口）
├─ audio_mp3.cpp（统一 MP3 核心）
├─ audio_mp3_source_file.cpp
├─ audio_mp3_source_audiotools.cpp
└─ audio_radio_backend.cpp

Storage
├─ storage_catalog_v3
├─ storage_index_v3
├─ storage_scan_v3
├─ storage_builder_v3
├─ storage_groups_v3
└─ net_music_catalog
```

### 可借鉴设计

1. **音频首响优先原则**
   - 切歌时先启动音频，封面/歌词延后加载
   - 不要把封面缩放等高开销操作和音频启动混在一起

2. **统一 MP3 解码核心**
   - 本地文件、网络电台、NAS HTTP 歌曲最终都走 `audio_mp3.cpp`
   - 通过 `AudioMp3Source` 适配不同输入源

3. **V3 音乐索引**
   - 启动优先加载 `/System/music_index_v3.bin`
   - 只有必要时才重扫 `/Music`
   - 包含 tracks / albums / artists / string_pool 结构化存储

4. **NAS 歌曲 offset 索引**
   - 1339 首歌曲只保存行偏移（约 5KB），不保存完整 URL
   - 播放或分页时通过 `seek(offset)` 按需读取

5. **无 CD 脚 TF 卡热插拔**
   - 无卡状态周期性尝试 mount
   - 有卡状态低频 probe alive
   - 本地播放中不主动 probe，通过 AudioFile IO error 检测拔卡
   - 阻止拔卡后连续 auto next

6. **SdFat SHARED_SPI 配置**
   - 多任务访问 SD 时使用 `SHARED_SPI`，避免 `DEDICATED_SPI` 跨任务 transaction 导致 FreeRTOS mutex assert

7. **按 TF 卡区分 snapshot**
   - 读取 TF 卡 CID 计算 hash，生成卡专属 snapshot key
   - 不同 TF 卡保存独立播放状态

8. **网络播放源独立状态**
   - `LOCAL_TRACK` / `NET_RADIO` / `NET_TRACK` 三个独立源
   - 不混用 snapshot，不互相污染

9. **播放模式设计**
   - 大类：全部 / 歌手 / 专辑
   - 小类：顺序 / 随机
   - 组合成 6 个模式

10. **NAS 随机播放互质步长序列**
    - `index = (shuffle_start + shuffle_pos * shuffle_step) % track_count`
    - 一轮内尽量不重复，无需保存完整随机数组

### 关键文件

- `README.md` — 项目完整说明
- `ESP32-S3 音乐播放器项目阶段复盘报告（V1.0）.md` — 问题复盘与经验沉淀
- `ESP32S3_music_player_project_summary.md` — 项目当前状态总结
- `include/app_state.h` / `src/app_state.cpp` — 应用状态机
- `include/player_*.h` / `src/player_*.cpp` — 播放器核心
- `include/audio/` / `src/audio/` — 音频系统
- `include/storage/` / `src/storage/` — 存储与索引
- `include/ui/` / `src/ui/` — UI 渲染
- `include/web/` / `src/web/` — Web 控制

### 与本项目的差异

| 方面 | AppState 项目 | 灵镜 AI 音响 |
|------|---------------|--------------|
| 框架 | PlatformIO + Arduino | ESP-IDF + LVGL |
| 屏幕 | GC9A01 圆屏 240×240 | ILI9341 2.8 寸 240×320 横屏 320×240 |
| 显示库 | LovyanGFX | LVGL |
| 触摸屏 | 无（按键 + 旋钮） | FT6336G 电容屏 |
| 音频 DAC | PCM5102A | ES8311 + FM8002E |
| 输入 | 6 按键 + 旋钮 | 电容触摸 |

---

## 项目二：ESP32_LVGL_MusicPlayer — LVGL 音乐播放器

### 定位

基于 **LVGL + TFT_eSPI** 的音乐播放器示例，结构简单，最接近我们目标技术栈，可作为驱动对接和 UI 循环的入门参考。

### 核心能力

- TFT_eSPI 初始化与 LVGL 对接
- XPT2046 电阻触摸屏输入
- 音频播放（封装在 `Music` 库中）
- 播放/暂停、上一首/下一首、歌词显示
- 播放进度条、时间显示
- 黑胶唱片旋转动画

### 关键代码

文件：`src/main.cpp`

#### 1. LVGL 显示刷新回调

```cpp
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}
```

#### 2. 触摸屏坐标映射

```cpp
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  TS_Point p = xpt_touch.getPoint();
  bool touched = xpt_touch.touched();

  if (touched) {
    touchX = map(p.x, 320, 3700, 0, 240);
    touchY = map(p.y, 300, 3820, 0, 320);
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}
```

#### 3. LVGL 初始化流程

```cpp
lv_init();
tft.begin();
tft.setRotation(0);
xpt_touch.begin();
xpt_touch.setRotation(0);

lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 15);

static lv_disp_drv_t disp_drv;
lv_disp_drv_init(&disp_drv);
disp_drv.hor_res = screenWidth;
disp_drv.ver_res = screenHeight;
disp_drv.flush_cb = my_disp_flush;
disp_drv.draw_buf = &draw_buf;
lv_disp_drv_register(&disp_drv);

static lv_indev_drv_t indev_drv;
lv_indev_drv_init(&indev_drv);
indev_drv.type = LVINDEV_TYPE_POINTER;
indev_drv.read_cb = my_touchpad_read;
lv_indev_drv_register(&indev_drv);

ui_init();
```

#### 4. 音频任务分离

```cpp
xTaskCreatePinnedToCore(audioTask, "audioTask", 8192, NULL,
                        configMAX_PRIORITIES - 1, &audioTaskHandle, 0);
```

音频在 CPU0 运行，UI 在主循环 CPU1 运行。

### 可借鉴设计

1. **LVGL + TFT_eSPI 对接模式**：显示 flush、输入设备注册流程可直接参考
2. **双核任务分离**：音频任务 pinned 到 CPU0，UI 在主循环
3. **UI 更新循环**：`lv_timer_handler()` + `UI_update()` 的结构
4. **歌词同步逻辑**：按时间戳匹配当前歌词行
5. **播放模式处理**：列表循环、单曲循环、随机播放

### 需要适配的地方

| 项目二 | 灵镜 AI 音响 |
|--------|--------------|
| XPT2046 电阻触摸 | FT6336G 电容触摸（I2C） |
| 240×320 竖屏 | 320×240 横屏（需旋转） |
| Music 库未开源 | 需自行集成 ESP32 音频解码 |
| 黑胶唱片 UI | 可复用思路，但需改为横屏布局 |

---

## 项目三：xiaozhi-esp32-music — 小智 AI 音乐固件

### 定位

虾哥开源的小智 AI 聊天机器人音乐扩展固件，基于 **ESP-IDF**，核心特征是语音交互 + MCP 协议 + 在线音乐服务。

### 核心能力

- 语音 AI 交互入口
- 通过 MCP 协议调用 `self.music.play_song` 播放音乐
- Wi-Fi / 4G 网络连接
- 离线语音唤醒（ESP-SR）
- Websocket / MQTT+UDP 通信
- OPUS 音频编解码
- 多开发板支持（ESP32-S3 / C3 / C6 / P4）

### 与本项目的相关性

| 方面 | 小智项目 | 灵镜 AI 音响 |
|------|----------|--------------|
| 框架 | ESP-IDF | ESP-IDF ✅ 一致 |
| 核心定位 | AI 语音聊天 + 音乐 | 触屏音乐播放器 |
| 交互方式 | 语音为主 | 触摸屏为主 |
| 音频 | OPUS + 在线音乐 | MP3 + 本地/网络 |
| 网络协议 | WebSocket / MQTT | 可简化或后期扩展 |

### 可借鉴部分

1. **ESP-IDF 工程结构**：`main/CMakeLists.txt`、`idf_component.yml`、`sdkconfig.defaults.*` 的写法
2. **音频服务实现**：`main/audio/audio_service.cc` 可作为 I2S 音频输出路由参考
3. **开发板抽象**：`main/boards/common/` 的板级抽象思路
4. **OTA 和系统信息模块**：后续扩展系统功能时参考

### 暂不直接复用的部分

- MCP 协议、LLM 交互、语音唤醒等 AI 功能与当前触屏播放器目标偏离
- 在线音乐服务依赖小智后端，不适合作为本地播放器基础

---

## 综合建议：灵镜 AI 音响如何借鉴

### 第一阶段：驱动与 LVGL 跑通

**主要参考项目二 + 项目一的架构思想**

1. 使用 ESP-IDF + LVGL，参考项目二的 `lv_disp_drv_t` / `lv_indev_drv_t` 注册流程
2. 显示驱动使用 ILI9341（SPI），参考项目一的 LovyanGFX 配置或项目二的 TFT_eSPI 配置
3. 触摸驱动使用 FT6336G（I2C），参考项目二的输入设备抽象，但改为 I2C 读取
4. 屏幕旋转为 320×240 横屏，ILI9341 `MADCTL` 设置 + 触摸坐标 90° 映射

### 第二阶段：音频播放

**主要参考项目一 + 项目三**

1. 优先实现本地 MicroSD 卡 MP3 播放
2. 音频服务独立任务 + 命令队列（参考项目一 `audio_service`）
3. I2S 输出到 ES8311（参考项目三 `audio_service`）
4. 后期可加入 HTTP 网络电台 / NAS HTTP MP3（参考项目一 `audio_mp3.cpp` + URLStream）

### 第三阶段：UI 页面

**参考项目二 + 项目一**

1. 播放页：封面、歌曲名、歌手、进度条、播放控制
2. 歌单页：列表滚动、选中高亮
3. 电台页：电台列表、切换
4. 搜索页：可后期扩展，先做本地搜索

### 第四阶段：存储与索引

**参考项目一**

1. 建立 `/Music` 目录扫描
2. 建立 V3 二进制索引 `/System/music_index_v3.bin`
3. 使用字符串池去重 artist/album
4. 大列表 offset 索引

### 第五阶段：网络功能（可选）

**参考项目一 + 项目三**

1. 从 SD 卡读取 `/System/config/wifi.conf`
2. Web 控制页面（参考项目一的 Web API 设计）
3. 网络电台列表 `/System/radio_list.txt`
4. NAS HTTP 歌曲 `/System/net_music.txt`

---

## 可直接复用的文件/代码清单

| 来源项目 | 文件/模块 | 复用方式 |
|----------|-----------|----------|
| 项目一 | `player_*.cpp/h` 架构思想 | 在 components/music_player 中实现我们自己的状态机 |
| 项目一 | `audio_service` 设计 | 独立音频任务 + 命令队列 |
| 项目一 | V3 索引设计 | 实现简化版 music_index |
| 项目一 | 热插拔处理思路 | 实现 storage hotplug |
| 项目二 | `main.cpp` LVGL 初始化 | 移植到 `main/main.c` |
| 项目二 | UI 更新循环 | `lv_timer_handler()` + UI_update() |
| 项目三 | `audio_service.cc` | 参考 I2S 配置和输出路由 |
| 项目三 | `sdkconfig.defaults.esp32s3` | 参考 ESP32-S3 默认配置 |

---

## 注意事项

1. **框架差异**：项目一/二是 PlatformIO + Arduino，项目三/灵镜是 ESP-IDF，不能直接复制代码，需要按 ESP-IDF 风格重写。
2. **硬件差异**：屏幕驱动、触摸芯片、音频 DAC 都不同，引脚定义必须按 ES3C28P 修改。
3. **内存管理**：ESP32-S3 有 8MB PSRAM，大缓冲优先放 PSRAM，内部 RAM 留给热路径。
4. **版权与许可证**：参考项目多为 MIT/GPL 开源，复用代码时需注意许可证声明。
