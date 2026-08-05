# 灵镜 AI 音响 — 硬件资料

## 开发板型号

- **SKU**：ES3C28P（带电容触摸屏版本）
- **官方 Wiki**：[2.8inch ESP32-S3 Display](https://www.lcdwiki.com/zh/2.8inch_ESP32-S3_Display)

## 主控

| 名称 | 参数 |
|------|------|
| 模组 | ESP32-S3（N16R8） |
| CPU | Xtensa LX7 32 位双核处理器 |
| 主频 | 240 MHz（最大） |
| 存储 | 384 KB ROM + 512 KB SRAM + 16 KB RTC SRAM + 8 MB OPI PSRAM + 16 MB QSPI Flash |
| Wi-Fi | 2.4 GHz，802.11 b/g/n |
| 蓝牙 | 蓝牙 5.0 BR/EDR + BLE |
| 工作电压 | 3.0 ~ 3.6 V |

## 液晶屏

| 名称 | 参数 |
|------|------|
| 尺寸 | 2.8 inch |
| 类型 | IPS TFT |
| 分辨率 | 240 × 320（竖屏） |
| 驱动 IC | ILI9341V |
| 显示接口 | 4-Line SPI |
| 颜色 | 最大 262K（RGB666），常用 65K（RGB565） |
| 背光 | White LED × 4，亮度约 230 cd/m² |

## 触摸屏

| 名称 | 参数 |
|------|------|
| 类型 | 电容触摸屏 |
| 分辨率 | 240 × 320 |
| 驱动 IC | FT6336G |
| 通信接口 | I2C |

## 音频

| 名称 | 参数 |
|------|------|
| 音频编解码 | ES8311 |
| 音频功放 | FM8002E |
| 麦克风 | LMA2718B381-OA7（MEMS MIC） |
| 接口 | 喇叭 1.25mm 2P 座子 |

## 存储

- **MicroSD 卡槽**：1 个，用于扩展存储（字库、图片、音频等）

## 其他外设

- **RGB 三色灯**：WS2812B，单线控制
- **按键**：BOOT（IO0）、RESET（EN）
- **电池**：3.7V 聚合物锂电池 + TP4054 充电管理
- **供电接口**：USB Type-C

## ESP32-S3 GPIO 分配

| 板载设备 | ESP32 引脚 | 说明 |
|----------|-----------|------|
| **液晶屏** | IO10 | 片选 CS，低电平有效 |
| | IO46 | 命令/数据选择 DC/RS |
| | IO12 | SPI 时钟 SCK |
| | IO11 | SPI 写数据 MOSI |
| | IO13 | SPI 读数据 MISO |
| | EN | 复位 RST，低电平复位 |
| | IO45 | 背光控制 BL，高电平点亮 |
| **电容触摸屏** | IO16 | I2C SDA |
| | IO15 | I2C SCL |
| | IO18 | 复位 RST，低电平复位 |
| | IO17 | 中断 INT，触摸时低电平 |
| **RGB 灯** | IO42 | WS2812B 单线数据 |
| **MicroSD 卡** | IO38 | SDIO CLK |
| | IO40 | SDIO CMD |
| | IO39 / IO41 / IO48 / IO47 | SDIO DATA0 ~ DATA3 |
| **音频 ES8311** | IO1 | 音频使能，低电平使能 |
| | IO4 | I2S MCLK |
| | IO5 | I2S BCLK |
| | IO6 | I2S DOUT |
| | IO7 | I2S LRCK（WS） |
| | IO8 | I2S DIN |
| **按键** | IO0 | BOOT 按键 |
| | EN | RESET 按键 |
| **串口** | IO43 | UART0 RX |
| | IO44 | UART0 TX |
| **电池** | IO9 | 电池电压 ADC 检测 |
| **扩展接口** | IO2 / IO3 / IO14 / IO21 | 普通 GPIO |

## 关键设计信息

- 屏幕物理分辨率为 **240 × 320 竖屏**，UI 原型文档中的 320×240 为横屏预览，实际移植时需要注意方向适配。
- 触摸屏使用 **I2C 接口**，与 I2C 外设扩展接口共用 IO15 / IO16。
- 音频输出通过 **I2S + ES8311 + FM8002E** 功放驱动喇叭。
- 大容量音频文件建议存放在 **MicroSD 卡**。

## 参考资料

- [ILI9341V 数据手册](https://www.lcdwiki.com/res/E32R28T/ILI9341V_DataSheet.pdf)
- [FT6336G 数据手册](https://www.lcdwiki.com/res/PublicFile/D-FT6336G-DataSheet-V1.0.pdf)
- [ES8311 数据手册](https://www.lcdwiki.com/res/PublicFile/ES8311_DS.pdf)
- [FM8002E 数据手册](https://www.lcdwiki.com/res/PublicFile/FM8002E.pdf)
- [ESP32-S3 技术参考手册](https://www.lcdwiki.com/res/PublicFile/esp32-s3_technical_reference_manual_cn.pdf)
