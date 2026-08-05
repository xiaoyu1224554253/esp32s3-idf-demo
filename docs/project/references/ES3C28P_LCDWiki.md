## 产品图片

- ![](https://www.lcdwiki.com/images/d/d7/ES3C28P-03.png)
	ES3C28P-TopView
- ![](https://www.lcdwiki.com/images/3/35/ES3C28P-04.png)
	ES3C28P-BottomView

## 产品介绍

- 自带ESP32-S3，性能强大，开发方便，开发资源充足
- 2.8寸彩屏，240x320分辨率，最大支持262K色（RGB666）,显示色彩丰富
- 接口丰富，方便连接各种外设（IIC、UART等外设）
- 支持外接喇叭，自带麦克风，可以播放接收音频
- 自带RGB三色指示灯，指示状态丰富
- 自带电容触摸屏，方便人机交互
- 标准的TYPE-C接口，方便程序下载和供电
- 自带micro TF卡槽，方便扩展存储
- 支持外接锂电池，轻巧便携
- 自带电池充电管理电路，可确保电池安全充放电
- 提供丰富的示例程序，方便学习
- 提供底层驱动技术支持,WIKI资料在线更新
- 模块老化测试多重检测可达军工级标准，支持长期稳定工作
- 支持小智AI语音聊天

## 产品参数

## ESP32主控参数

| **名称** | **参数** |
| --- | --- |
| **模组** | ESP32-S3 |
| **CPU** | Xtensa LX7 32 位双核处理器 |
| **主频** | 240MHz（最大） |
| **存储** | 384 KB ROM + 512KB SRAM  +16 KB RTC SRAM + 8M 内置OPI PSRAM + 16M 外接QSPI Flash  （N16R8） |
| **WIFI** | 2.4GHz、802.11b/g/n 模式 |
| **蓝牙** | 蓝牙V5.0 BR/EDR和蓝牙LE标准 |
| **工作电压** | 3.0~3.6(V) |

## 液晶屏参数

| **名称** | **参数** |
| --- | --- |
| **屏幕尺寸** | 2.8 inch |
| **屏幕类型** | IPS TFT |
| **分辨率** | 240xRGBx320(pixels) |
| **有效显示区** | 43.20(W)x57.60(H)(mm) |
| **颜色数目** | 最大：262K(RGB666)  常用：65K(RGB565) |
| **驱动IC** | ILI9341V |
| **显示接口** | 4-Line SPI(接到ESP32上) |
| **像素尺寸** | 0.153(H)x0.153(mm) |
| **可视角度** | ALL 0’CLOCK |
| **背光亮度(典型值)** | 280 cd/m <sup>2</sup> |
| **背光灯类型** | White LED\*4 |
| **工作温度** | \-30~80(℃) |
| **存储温度** | \-30~80(℃) |

## 触摸屏参数

| **名称** | **参数** |
| --- | --- |
| **有效区尺寸** | 2.8 inch |
| **触摸屏类型** | 电容触摸屏 |
| **触摸屏分辨率** | 240x320(pixels) |
| **驱动IC** | FT6336G |
| **可视窗口尺寸** | 45.20(W)x59.45(H)(mm) |
| **通信接口** | IIC |
| **结构材质** | ITO膜+ITO玻璃 |
| **工作温度** | \-30~80(℃) |
| **存储温度** | \-30~80(℃) |

## 尺寸参数

| **名称** | **参数** |
| --- | --- |
| **液晶屏外形尺寸** | 50.00±0.2(W)x69.20±0.2(H)x2.3±0.1(D)(不包含排线和背胶) |
| **触摸屏外形尺寸** | 50.00±0.2(W)x69.20±0.2(H)x1.20 (D) ±0.1(D)(不包含排线和背胶) |
| **模块外形尺寸** | 有触摸屏：50.00(W)x86.00(H)x10.60(D)  无触摸屏：50.00(W)x86.00(H)x9.10(D) |

## 电池充电参数

| **名称** | **参数** |
| --- | --- |
| **充电电压** | 范围：4.2~6.5(V)  典型值：5V |
| **充电电流** | 最大值：500mA  模块实际值：290mA |
| **充电饱和电压** | 4.24V |
| **充电温度** | 模块实际最大值：62℃ |
| **充电电池规格** | 3.7V聚合锂电池 |

## 电气参数

| **名称** | **参数** |
| --- | --- |
| **工作电压** | 5.0V |
| **背光电流** | 79mA |
| **背光亮度(实际值)** | 有触摸屏：230cd/m <sup>2</sup>  无触摸屏：270cd/m <sup>2</sup> |
| **总电流** | ESP32复位：0  只有显示屏工作：140mA 显示屏、喇叭、电池充电都工作：560mA |
| **功耗** | 0.7（只有显示屏工作）  2.8（显示屏，喇叭，电池充电都工作） |

## 基本参数

| **名称** | **参数** |
| --- | --- |
| **SKU** | 有触摸屏：ES3C28P  无触摸屏：ES3N28P |
| **供电接口** | USB(Type-C) |
| **重量（包含包装）** | ES3C28P:111g  ES3N28P:100g |

## 接口定义

![](https://www.lcdwiki.com/images/thumb/c/cd/ES3C28P-05.png/956px-ES3C28P-05.png)

## 接口功能说明

| **接口** | **功能说明** |
| --- | --- |
| **ESP32-S3** | 显示模块主控，控制板载外设和外接外设。 |
| **MicroSD卡槽** | 插入Micro SD卡，用来扩展存储空间，例如存放字库、图片、音频文件等大数据内容。 |
| **RGB三色灯** | 包含红、绿、蓝三种颜色的LED灯，只需一个IO控制，用来指示状态。 |
| **串口** | 1.25mm 4P座子。可用于串口调试、下载以及通信。需外接USB转串口模块。 |
| **电池接口** | 1.25mm 2P座子，用于接入3.7V聚合锂电池，  通过电池充电管理电路对电池进行充电，也可用于电池供电。 注意接口正、负极。 |
| **BOOT按键** | 用于进入下载模式或者按键测试。  按住此按键上电，然后松开可进入下载模式，  或者上电后，按住此按键，再按RESET键，  松开RESET键后，再松开此按键，也可以进下载模式。  不需要进入下载模式时，此按键可做普通按键使用。 |
| **TYPE-C接口** | 用于模块供电和下载程序。 |
| **RESET按键** | 用于ESP32主控以及LCD复位，按下后电平复位。 |
| **扩展输入引脚** | 1.25mm 4P座子。引出GPIO2/3/14/21引脚 |
| **喇叭接口** | 1.25mm 2P座子。用于接入喇叭播放音频。 |
| **IIC外设接口** | 1.25mm 4P座子。用于外接IIC通信设备，此IIC接口和电容触摸屏共用。可做普通IO使用。 |

## ESP32引脚分配

<table><tbody><tr><th align="center"><b>板载设备</b></th><th align="center"><b>ESP32连接引脚</b></th><th align="center"><b>板载设备引脚说明</b></th></tr><tr><td rowspan="7" align="center"><b>液晶屏</b></td><td align="center">IO10</td><td>液晶屏片选控制信号，低电平有效</td></tr><tr><td align="center">IO46</td><td>液晶屏命令/数据选择控制信号<p>高电平：数据，低电平：命令</p></td></tr><tr><td align="center">IO12</td><td>液晶屏SPI总线时钟信号</td></tr><tr><td align="center">IO11</td><td>液晶屏SPI总线写数据信号</td></tr><tr><td align="center">IO13</td><td>液晶屏SPI总线读数据信号</td></tr><tr><td align="center">EN</td><td>液晶屏复位控制信号，低电平复位（和ESP32-32E主控共用复位引脚）</td></tr><tr><td align="center">IO45</td><td>液晶屏背光控制信号（高电平点亮背光，低电平关闭背光）</td></tr><tr><td rowspan="4" align="center"><b>电容触摸屏</b></td><td align="center">IO16</td><td>电容触摸屏I2C总线数据信号</td></tr><tr><td align="center">IO15</td><td>电容触摸屏I2C总线时钟信号</td></tr><tr><td align="center">IO18</td><td>电容触摸屏复位控制信号，低电平复位</td></tr><tr><td align="center">IO17</td><td>电容触摸屏中断输入信号，发生触摸事件时，输入低电平</td></tr><tr><td rowspan="1" align="center"><b>RGB三色灯</b></td><td align="center">IO42</td><td>单线RGB三色LED灯，可以根据不同信号分别控制内部的红绿蓝三种灯珠</td></tr><tr><td rowspan="3" align="center"><b>MicroSD 卡</b></td><td align="center">IO38</td><td>SD卡SDIO总线时钟信号</td></tr><tr><td align="center">IO40</td><td>SD卡SDIO总线命令信号</td></tr><tr><td align="center">IO39/41/48/47</td><td>SD卡SDIO总线数据信号（DATA0~DATA3四根数据线）</td></tr><tr><td rowspan="6" align="center"><b>音频</b></td><td align="center">IO1</td><td>音频使能信号，低电平使能，高电平禁止</td></tr><tr><td align="center">IO4</td><td>音频I2S总线主时钟信号</td></tr><tr><td align="center">IO5</td><td>音频I2S总线位时钟信号</td></tr><tr><td align="center">IO6</td><td>音频I2S总线位输出数据信号</td></tr><tr><td align="center">IO7</td><td>音频I2S总线左右声道选择信号。高电平：右声道；低电平：左声道</td></tr><tr><td align="center">IO8</td><td>音频I2S总线位输入数据信号</td></tr><tr><td rowspan="2" align="center"><b>按键</b></td><td align="center">IO0</td><td>下载模式选择按键（按住该按键上电，然后松开就会进入下载模式）</td></tr><tr><td align="center">EN</td><td>ESP32-S3复位按键，低电平复位（和液晶屏复位共用）</td></tr><tr><td rowspan="2" align="center"><b>串口</b></td><td align="center">RXD0(IO43)</td><td>ESP32-S3串口接收信号（如果不使用串口，可做普通IO使用）</td></tr><tr><td align="center">TXD0(IO44)</td><td>ESP32-S3串口发送信号（如果不使用串口，可做普通IO使用）</td></tr><tr><td align="center"><b>电池</b></td><td align="center">IO9</td><td>电池电压ADC值获取信号（输入）</td></tr><tr><td rowspan="4" align="center"><b>拓展接口</b></td><td align="center">IO2</td><td>可做普通IO使用</td></tr><tr><td align="center">IO3</td><td>可做普通IO使用</td></tr><tr><td align="center">IO14</td><td>可做普通IO使用</td></tr><tr><td align="center">IO21</td><td>可做普通IO使用</td></tr><tr><td rowspan="2" align="center">IIC外设扩展接口</td><td align="center">IO16</td><td>I2C总线数据信号</td></tr><tr><td align="center">IO15</td><td>I2C总线时钟信号</td></tr></tbody></table>

## 快速使用说明

[**2.8寸ESP32-S3显示模块快速使用资料包**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_IPS_ESP32-S3_ES3C28P_ES3N28P_Quick_Start.zip "en:res/ES3C28P/2.8inch IPS ESP32-S3 ES3C28P ES3N28P Quick Start.zip")

[**小智AI例程源码**](https://www.lcdwiki.com/res/ES3C28P/%E5%B0%8F%E6%99%BA%E4%BE%8B%E7%A8%8B%E6%BA%90%E7%A0%81.zip "en:res/ES3C28P/小智例程源码.zip")

[**2.8寸ESP32-32E显示模块快速使用说明**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_IPS_ESP32-S3_ES3C28P_ES3N28P%E5%BF%AB%E9%80%9F%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C.pdf "en:res/ES3C28P/2.8inch IPS ESP32-S3 ES3C28P ES3N28P快速使用手册.pdf")

[**在本产品上部署小智AI的使用说明**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_ESP32-S3%E5%B0%8F%E6%99%BAAI%E5%BF%AB%E9%80%9F%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C.pdf "en:res/ES3C28P/2.8inch ESP32-S3小智AI快速使用手册.pdf")

## 资料包下载

高速云盘分享链接1： [**2.8inch\_IPS\_ESP32-S3\_ILI9341V\_ES3C28P\_ES3N28P\_V1.0**](https://www.123865.com/s/Kg5Wvd-7UGr3)

百度云盘备份链接2： [**2.8寸ESP32-S3-ILI9341驱动显示模块资料包(提取码：yvne)**](https://pan.baidu.com/s/1G3M-Q1ztXdHwf7xHjxnRsQ?pwd=yvne)

高速云盘备份链接3： [**2.8inch ESP32-S3-isplay module data package**](https://1855123618.v.123pan.cn/1855123618/24207552)

## 产品文档

## 规格书

#### 产品规格书

[**2.8寸ESP32-S3显示模块产品规格书**](https://www.lcdwiki.com/res/ES3C28P/ES3C28P_ES3N28P%E4%BA%A7%E5%93%81%E8%A7%84%E6%A0%BC%E4%B9%A6_V1.0.pdf "en:res/ES3C28P/ES3C28P ES3N28P产品规格书 V1.0.pdf")

#### 液晶屏规格书

[**2.8寸QD2803屏规格书**](https://www.lcdwiki.com/res/ES3C28P/QD2833-Specifications-A.pdf "en:res/ES3C28P/QD2833-Specifications-A.pdf")

## 用户手册

[**2.8寸ESP32-S3显示模块用户手册**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_IPS_ESP32-S3_ES3C28P_ES3N28P%E7%94%A8%E6%88%B7%E6%89%8B%E5%86%8C.pdf "en:res/ES3C28P/2.8inch IPS ESP32-S3 ES3C28P ES3N28P用户手册.pdf")

## 尺寸图

[**ES3C28P显示模块尺寸图**](https://www.lcdwiki.com/res/ES3C28P/ES3C28P_Size.pdf "en:res/ES3C28P/ES3C28P Size.pdf")

[**ES3N28P显示模块尺寸图**](https://www.lcdwiki.com/res/ES3C28P/ES3N28P_Size.pdf "en:res/ES3C28P/ES3N28P Size.pdf")

## 3D图

[**ES3C28P显示模块3D图**](https://www.lcdwiki.com/res/ES3C28P/ES3C28P_3D.zip "en:res/ES3C28P/ES3C28P 3D.zip")

[**ES3N28P显示模块3D图**](https://www.lcdwiki.com/res/ES3C28P/ES3N28P_3D.zip "en:res/ES3C28P/ES3N28P 3D.zip")

## 原理图

[**2.8inch\_ESP32-S3显示模块硬件原理图**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_ESP32-S3%E6%98%BE%E7%A4%BA%E6%A8%A1%E5%9D%97%E7%A1%AC%E4%BB%B6%E5%8E%9F%E7%90%86%E5%9B%BE.pdf "en:res/ES3C28P/2.8inch ESP32-S3显示模块硬件原理图.pdf")

## IO资源分配表

[**ESP32-S3芯片IO资源分配表**](https://www.lcdwiki.com/res/ES3C28P/ESP32-S3%E8%8A%AF%E7%89%87IO%E8%B5%84%E6%BA%90%E5%88%86%E9%85%8D%E8%A1%A8.xlsx "en:res/ES3C28P/ESP32-S3芯片IO资源分配表.xlsx")

## 封装库

[**2.8inch\_IPS\_ESP32-S3\_Display\_AD封装库**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_IPS_ESP32-S3_Display_AD%E5%B0%81%E8%A3%85%E5%BA%93.zip "en:res/ES3C28P/2.8inch IPS ESP32-S3 Display AD封装库.zip")

## 液晶屏初始化代码

[**ILI9341初始化代码**](https://www.lcdwiki.com/res/ES3C28P/ILI9341V_Init.txt "en:res/ES3C28P/ILI9341V Init.txt")

## 参考资料

## 开发环境搭建

[**ESP32 Arduino IDE开发环境搭建说明**](https://www.lcdwiki.com/res/PublicFile/ESP32_Arduino_IDE%E5%BC%80%E5%8F%91%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA.pdf "en:res/PublicFile/ESP32 Arduino IDE开发环境搭建.pdf")

[**ESP32 MicroPython开发环境搭建说明**](https://www.lcdwiki.com/res/PublicFile/ESP32_MicroPython%E5%BC%80%E5%8F%91%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA.pdf "en:res/PublicFile/ESP32 MicroPython开发环境搭建.pdf")

[**ESP32 ESP-IDF使用VSCODE开发环境搭建说明**](https://www.lcdwiki.com/res/PublicFile/%E4%BD%BF%E7%94%A8VSCode%E6%90%AD%E5%BB%BAESP-IDF%E7%8E%AF%E5%A2%83.pdf "en:res/PublicFile/使用VSCode搭建ESP-IDF环境.pdf")

[**ESP32 ESP-IDF LVGL移植说明**](https://www.lcdwiki.com/res/PublicFile/ESP-IDF_LVGL%E7%A7%BB%E6%A4%8D%E8%AF%B4%E6%98%8E.pdf "en:res/PublicFile/ESP-IDF LVGL移植说明.pdf")

## 示例代码说明

[**2.8寸ESP32-S3显示模块Arduino示例程序说明**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_ES3C28P_ES3N28P_arduino%E7%A4%BA%E4%BE%8B%E7%A8%8B%E5%BA%8F%E8%AF%B4%E6%98%8E.pdf "en:res/ES3C28P/2.8inch ES3C28P ES3N28P arduino示例程序说明.pdf")

[**2.8寸ESP32-S3显示模块MicroPython示例程序说明**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_ES3C28P_ES3N28P_MicroPython%E7%A4%BA%E4%BE%8B%E7%A8%8B%E5%BA%8F%E8%AF%B4%E6%98%8E.pdf "en:res/ES3C28P/2.8inch ES3C28P ES3N28P MicroPython示例程序说明.pdf")

[**2.8寸ESP32-S3显示模块ESP-IDF示例程序说明**](https://www.lcdwiki.com/res/ES3C28P/2.8inch_ES3C28P_ES3N28P_ESP-IDF%E7%A4%BA%E4%BE%8B%E7%A8%8B%E5%BA%8F%E8%AF%B4%E6%98%8E.pdf "en:res/ES3C28P/2.8inch ES3C28P ES3N28P ESP-IDF示例程序说明.pdf")

## 数据手册

[**ILI9341V数据手册**](https://www.lcdwiki.com/res/E32R28T/ILI9341V_DataSheet.pdf "en:res/E32R28T/ILI9341V DataSheet.pdf")

[**FT6336G-数据手册**](https://www.lcdwiki.com/res/PublicFile/D-FT6336G-DataSheet-V1.0.pdf "en:res/PublicFile/D-FT6336G-DataSheet-V1.0.pdf")

[**ESP32-S3芯片数据手册**](https://www.lcdwiki.com/res/PublicFile/esp32-s3_datasheet_cn.pdf "en:res/PublicFile/esp32-s3 datasheet cn.pdf")

[**ESP32-S3硬件设计指南**](https://www.lcdwiki.com/res/PublicFile/esp32-s3_hardware_design_guidelines_cn.pdf "en:res/PublicFile/esp32-s3 hardware design guidelines cn.pdf")

[**ESP32-S3技术参考手册**](https://www.lcdwiki.com/res/PublicFile/esp32-s3_technical_reference_manual_cn.pdf "en:res/PublicFile/esp32-s3 technical reference manual cn.pdf")

[**电池充电管理TP4054数据手册**](https://www.lcdwiki.com/res/PublicFile/TP4054.PDF "en:res/PublicFile/TP4054.PDF")

[**稳压管ME6217数据手册**](https://www.lcdwiki.com/res/PublicFile/ME6217_LDO.pdf "en:res/PublicFile/ME6217 LDO.pdf")

[**音频解码ES8311\_DS数据手册**](https://www.lcdwiki.com/res/PublicFile/ES8311_DS.pdf "en:res/PublicFile/ES8311 DS.pdf")

[**音频功放FM8002E数据手册**](https://www.lcdwiki.com/res/PublicFile/FM8002E.pdf "en:res/PublicFile/FM8002E.pdf")

[**MEMS\_MIC\_LMA2718B381-OA7数据手册**](https://www.lcdwiki.com/res/PublicFile/MEMS_MIC_LMA2718B381-OA7.PDF "en:res/PublicFile/MEMS MIC LMA2718B381-OA7.PDF")

[**RGB+LED(IC)数据手册**](https://www.lcdwiki.com/res/PublicFile/RGB%2BLED\(IC\)_WS2812B-V5-W.PDF "en:res/PublicFile/RGB+LED(IC) WS2812B-V5-W.PDF")

[**中英文取模设置**](https://www.lcdwiki.com/zh/%E3%80%90%E6%95%99%E7%A8%8B%E3%80%91%E4%B8%AD%E8%8B%B1%E6%96%87%E6%98%BE%E7%A4%BA%E5%8F%96%E6%A8%A1%E8%AE%BE%E7%BD%AE "en:zh/【教程】中英文显示取模设置")

## 工具软件

[**Flash\_Download\_Tool（官方下载）**](https://dl.espressif.com/public/flash_download_tool.zip) [**Flash\_Download\_Tool**](https://www.lcdwiki.com/res/software/Flash_Download.zip "en:res/software/Flash Download.zip")

[**JPGCompact**](https://www.lcdwiki.com/res/software/JPGCompact_V5.0.zip "en:res/software/JPGCompact V5.0.zip")

[**TCP\_UDP测试工具**](https://www.lcdwiki.com/res/software/TCP_UDP%E6%B5%8B%E8%AF%95%E5%B7%A5%E5%85%B7.zip "en:res/software/TCP UDP测试工具.zip")

[**串口调试助手**](https://www.lcdwiki.com/res/software/%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%8A%A9%E6%89%8B.zip "en:res/software/串口调试助手.zip")

[**网络调试助手**](https://www.lcdwiki.com/res/software/%E7%BD%91%E7%BB%9C%E8%B0%83%E8%AF%95%E5%8A%A9%E6%89%8B.zip "en:res/software/网络调试助手.zip")

[**esptouch**](https://www.lcdwiki.com/res/software/esptouch-v2.0.0.apk "en:res/software/esptouch-v2.0.0.apk")

[**PCtoLCD2002**](https://www.lcdwiki.com/res/software/PCtoLCD2002.zip "en:res/software/PCtoLCD2002.zip")

[**Image2Lcd**](https://www.lcdwiki.com/res/software/Image2Lcd.zip "en:res/software/Image2Lcd.zip")