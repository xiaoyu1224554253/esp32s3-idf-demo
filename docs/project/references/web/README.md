# 网页端参考素材

本目录存放从网页端收集的音乐播放器参考实现，供灵镜 AI 音响项目做 UI、交互与网络音频源设计时参考。

## 文件清单

| 文件 | 说明 | 可借鉴内容 |
|------|------|------------|
| [chinese_network_radio_player.html](./chinese_network_radio_player.html) | 中国网络电台播放器网页版 | 电台列表、播放逻辑、HTML Audio 使用方式 |
| [cloud_music_player.html](./cloud_music_player.html) | 云音乐在线播放器网页版 | 搜索页、播放页、歌词同步、下载功能 UI |
| [xinghai_music_source.js](./xinghai_music_source.js) | 星海音乐源（LX Music 插件） | 多平台音乐源映射、音质降级策略、API 调用方式 |

## 关键 API 信息（云音乐 / 星海音乐源）

> 注意：以下 API 为第三方聚合接口，仅做参考。ESP32-S3 若需在线音源，需要评估网络稳定性与接口可用性。

- **API 基地址**：`https://music-api.gdstudio.xyz/api.php`
- **支持平台**：网易云音乐、QQ 音乐、酷我音乐、酷狗音乐、咪咕音乐

### 常用接口

| 类型 | 参数示例 | 返回值 |
|------|----------|--------|
| 搜索 | `types=search&source=netease&name=关键词&count=30` | 歌曲列表 |
| 获取播放链接 | `types=url&source=netease&id=歌曲ID&br=320` | 音频直链 |
| 获取专辑图 | `types=pic&source=netease&id=图片ID&size=300` | 图片链接 |
| 获取歌词 | `types=lyric&source=netease&id=歌曲ID` | LRC 歌词 |

### 音质映射

| 音质标识 | 含义 |
|----------|------|
| `128` | 标准 128K |
| `192` | 较高 192K |
| `320` | 高品质 320K |
| `740` | 无损 FLAC |
| `999` | Hi-Res |

### 平台映射

| 简写 | 平台 |
|------|------|
| `wy` / `netease` | 网易云音乐 |
| `tx` / `tencent` | QQ 音乐 |
| `kw` / `kuwo` | 酷我音乐 |
| `kg` / `kugou` | 酷狗音乐 |
| `mg` / `migu` | 咪咕音乐 |

## 对灵镜 AI 音响项目的价值

1. **UI 参考**：cloud_music_player.html 的深色主题、搜索列表、播放控制、歌词显示可直接作为 LVGL 界面设计灵感。
2. **在线音源设计**：xinghai_music_source.js 展示了如何封装多平台音源、处理音质降级和频率限制。
3. **电台功能参考**：chinese_network_radio_player.html 可作为后续"电台"页面的数据来源参考。

## 限制说明

- 这些网页应用依赖浏览器环境和外部网络 API，**不能直接运行在 ESP32-S3 上**。
- 第三方 API 可能存在访问限制、频率限制或随时失效的风险。
- 如要在设备上实现网络音乐播放，需要自行部署稳定的后端，或改用本地 MicroSD 卡播放方案。
