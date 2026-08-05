# 阶段 0-3：BSP + 显示 + 触摸 + LVGL 移植实施计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 完成 ES3C28P 开发板的 BSP 组件搭建，点亮 ILI9341 屏幕，实现 FT6336G 触摸读取与横屏坐标映射，完成 LVGL 移植并显示测试页面。

**Architecture:** BSP 层（`components/bsp`）封装所有硬件驱动，提供统一初始化入口；LVGL 移植层（`components/lvgl_port`）将 BSP 的显示和触摸注册为 LVGL 的 `display` 和 `indev`；`main` 中调用 `lvgl_port_init()` 和 `lvgl_port_test()` 完成验证。

**Tech Stack:** ESP-IDF v6.0.2, LVGL v9, esp_lcd_ili9341, esp_lvgl_port

---

## 当前项目状态

- 已存在：`main/main.c`、`components/music_player/`、`CMakeLists.txt`
- 开发板：ES3C28P，ILI9341 240×320 竖屏，按横屏 320×240 使用
- 参考：Freenove 板级代码已导入 `docs/project/references/esp32/freenove-esp32s3-display-2.8-lcd/`

---

## 文件结构（本阶段）

```
components/
├── bsp/
│   ├── include/
│   │   ├── bsp_board.h
│   │   ├── bsp_lcd.h
│   │   ├── bsp_touch.h
│   │   └── bsp_backlight.h
│   ├── src/
│   │   ├── bsp_board.c
│   │   ├── bsp_lcd.c
│   │   ├── bsp_touch.c
│   │   └── bsp_backlight.c
│   └── CMakeLists.txt
└── lvgl_port/
    ├── include/
    │   └── lvgl_port.h
    ├── src/
    │   └── lvgl_port.c
    └── CMakeLists.txt
```

---

## 阶段 0：BSP 组件基础

### Task 0.1: 创建 BSP 目录和 CMakeLists.txt

**Files:**
- Create: `components/bsp/CMakeLists.txt`
- Create: `components/bsp/include/bsp_board.h`
- Create: `components/bsp/src/bsp_board.c`
- Modify: `main/CMakeLists.txt`

- [ ] **Step 1: 创建 bsp 组件 CMakeLists.txt**

```cmake
idf_component_register(
    SRCS "src/bsp_board.c"
         "src/bsp_lcd.c"
         "src/bsp_touch.c"
         "src/bsp_backlight.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_lcd esp_timer
)
```

- [ ] **Step 2: 创建 bsp_board.h**

```c
#ifndef BSP_BOARD_H
#define BSP_BOARD_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_board_init(void);

#ifdef __cplusplus
}
#endif

#endif // BSP_BOARD_H
```

- [ ] **Step 3: 创建 bsp_board.c**

```c
#include "bsp_board.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_backlight.h"
#include "esp_log.h"

static const char *TAG = "bsp_board";

esp_err_t bsp_board_init(void)
{
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Initializing BSP");

    ret = bsp_backlight_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "backlight init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = bsp_lcd_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "lcd init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = bsp_touch_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "touch init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "BSP initialized");
    return ESP_OK;
}
```

- [ ] **Step 4: 修改 main/CMakeLists.txt 添加 bsp 依赖**

```cmake
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES music_player bsp lvgl_port)
```

- [ ] **Step 5: 编译验证**

```bash
idf.py build
```

Expected: 编译成功（当前 bsp_lcd.c 等文件不存在，但 CMakeLists.txt 已声明；为避免错误，需要同时创建空文件或继续 Task 0.2 后再编译）。

> **注意**：如果 CMake 因缺少源文件报错，先创建空的 `src/bsp_lcd.c`、`src/bsp_touch.c`、`src/bsp_backlight.c` 占位，待后续任务填充。

- [ ] **Step 6: Commit**

```bash
git add components/bsp/ main/CMakeLists.txt
git commit -m "feat(bsp): create bsp component skeleton"
```

---

### Task 0.2: 添加 BSP 公共引脚定义

**Files:**
- Create: `components/bsp/include/bsp_pins.h`

- [ ] **Step 1: 创建 bsp_pins.h**

```c
#ifndef BSP_PINS_H
#define BSP_PINS_H

#include "driver/gpio.h"

// Audio I2S
#define BSP_AUDIO_I2S_MCLK_GPIO     GPIO_NUM_4
#define BSP_AUDIO_I2S_BCLK_GPIO     GPIO_NUM_5
#define BSP_AUDIO_I2S_WS_GPIO       GPIO_NUM_7
#define BSP_AUDIO_I2S_DOUT_GPIO     GPIO_NUM_8
#define BSP_AUDIO_I2S_DIN_GPIO      GPIO_NUM_6
#define BSP_AUDIO_PA_GPIO           GPIO_NUM_1

// Audio Codec I2C
#define BSP_AUDIO_I2C_NUM           I2C_NUM_0
#define BSP_AUDIO_I2C_SCL_GPIO      GPIO_NUM_15
#define BSP_AUDIO_I2C_SDA_GPIO      GPIO_NUM_16

// LCD SPI
#define BSP_LCD_SPI_HOST            SPI3_HOST
#define BSP_LCD_CS_GPIO             GPIO_NUM_10
#define BSP_LCD_SCK_GPIO            GPIO_NUM_12
#define BSP_LCD_MOSI_GPIO           GPIO_NUM_11
#define BSP_LCD_MISO_GPIO           GPIO_NUM_13
#define BSP_LCD_DC_GPIO             GPIO_NUM_46
#define BSP_LCD_RST_GPIO            GPIO_NUM_NC
#define BSP_LCD_BL_GPIO             GPIO_NUM_45

// Touch I2C
#define BSP_TOUCH_I2C_NUM           I2C_NUM_0
#define BSP_TOUCH_I2C_SCL_GPIO      GPIO_NUM_15
#define BSP_TOUCH_I2C_SDA_GPIO      GPIO_NUM_16
#define BSP_TOUCH_RST_GPIO          GPIO_NUM_18
#define BSP_TOUCH_INT_GPIO          GPIO_NUM_17
#define BSP_TOUCH_I2C_ADDR          0x38

// Display parameters
#define BSP_LCD_HOR_RES             320
#define BSP_LCD_VER_RES             240
#define BSP_LCD_SWAP_XY             true
#define BSP_LCD_MIRROR_X            false
#define BSP_LCD_MIRROR_Y            false
#define BSP_LCD_INVERT_COLOR        true
#define BSP_LCD_RGB_ORDER           LCD_RGB_ELEMENT_ORDER_BGR
#define BSP_LCD_SPI_CLOCK_HZ        (20 * 1000 * 1000)
#define BSP_LCD_SPI_MODE            0

// Boot button
#define BSP_BOOT_BUTTON_GPIO        GPIO_NUM_0

#endif // BSP_PINS_H
```

- [ ] **Step 2: 修改 bsp_board.c 包含 bsp_pins.h**

在 `bsp_board.c` 顶部添加：

```c
#include "bsp_pins.h"
```

- [ ] **Step 3: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 4: Commit**

```bash
git add components/bsp/
git commit -m "feat(bsp): add board pin definitions"
```

---

## 阶段 1：ILI9341 显示驱动

### Task 1.1: 实现背光 PWM 驱动

**Files:**
- Create: `components/bsp/include/bsp_backlight.h`
- Create: `components/bsp/src/bsp_backlight.c`

- [ ] **Step 1: 创建 bsp_backlight.h**

```c
#ifndef BSP_BACKLIGHT_H
#define BSP_BACKLIGHT_H

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_backlight_init(void);
esp_err_t bsp_backlight_set(uint8_t brightness_percent);

#ifdef __cplusplus
}
#endif

#endif // BSP_BACKLIGHT_H
```

- [ ] **Step 2: 创建 bsp_backlight.c**

```c
#include "bsp_backlight.h"
#include "bsp_pins.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "bsp_backlight";

#define BACKLIGHT_LEDC_MODE         LEDC_LOW_SPEED_MODE
#define BACKLIGHT_LEDC_CHANNEL      LEDC_CHANNEL_0
#define BACKLIGHT_LEDC_TIMER        LEDC_TIMER_0
#define BACKLIGHT_LEDC_FREQ_HZ      5000
#define BACKLIGHT_LEDC_DUTY_RES     LEDC_TIMER_8_BIT

esp_err_t bsp_backlight_init(void)
{
    ledc_timer_config_t timer_cfg = {
        .speed_mode       = BACKLIGHT_LEDC_MODE,
        .duty_resolution  = BACKLIGHT_LEDC_DUTY_RES,
        .timer_num        = BACKLIGHT_LEDC_TIMER,
        .freq_hz          = BACKLIGHT_LEDC_FREQ_HZ,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

    ledc_channel_config_t channel_cfg = {
        .gpio_num       = BSP_LCD_BL_GPIO,
        .speed_mode     = BACKLIGHT_LEDC_MODE,
        .channel        = BACKLIGHT_LEDC_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = BACKLIGHT_LEDC_TIMER,
        .duty           = 0,
        .hpoint         = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_cfg));

    ESP_LOGI(TAG, "backlight initialized on GPIO %d", BSP_LCD_BL_GPIO);
    return ESP_OK;
}

esp_err_t bsp_backlight_set(uint8_t brightness_percent)
{
    if (brightness_percent > 100) {
        brightness_percent = 100;
    }
    uint32_t duty = (brightness_percent * 255) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL));
    ESP_LOGI(TAG, "backlight set to %u%%", brightness_percent);
    return ESP_OK;
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
git commit -m "feat(bsp): add pwm backlight driver"
```

---

### Task 1.2: 实现 ILI9341 LCD 驱动

**Files:**
- Create: `components/bsp/include/bsp_lcd.h`
- Create: `components/bsp/src/bsp_lcd.c`

- [ ] **Step 1: 创建 bsp_lcd.h**

```c
#ifndef BSP_LCD_H
#define BSP_LCD_H

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_lcd_init(void);
esp_err_t bsp_lcd_fill_screen(uint16_t color);

#ifdef __cplusplus
}
#endif

#endif // BSP_LCD_H
```

- [ ] **Step 2: 创建 bsp_lcd.c**

```c
#include "bsp_lcd.h"
#include "bsp_pins.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

static const char *TAG = "bsp_lcd";

static esp_lcd_panel_io_handle_t panel_io = NULL;
static esp_lcd_panel_handle_t panel = NULL;

esp_err_t bsp_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    // SPI bus configuration
    spi_bus_config_t buscfg = {
        .mosi_io_num     = BSP_LCD_MOSI_GPIO,
        .miso_io_num     = BSP_LCD_MISO_GPIO,
        .sclk_io_num     = BSP_LCD_SCK_GPIO,
        .quadwp_io_num   = GPIO_NUM_NC,
        .quadhd_io_num   = GPIO_NUM_NC,
        .max_transfer_sz = BSP_LCD_HOR_RES * BSP_LCD_VER_RES * sizeof(uint16_t),
    };
    ret = spi_bus_initialize(BSP_LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "spi bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Panel IO configuration
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num       = BSP_LCD_CS_GPIO,
        .dc_gpio_num       = BSP_LCD_DC_GPIO,
        .spi_mode          = BSP_LCD_SPI_MODE,
        .pclk_hz           = BSP_LCD_SPI_CLOCK_HZ,
        .trans_queue_depth = 10,
        .lcd_cmd_bits      = 8,
        .lcd_param_bits    = 8,
    };
    ret = esp_lcd_new_panel_io_spi(BSP_LCD_SPI_HOST, &io_config, &panel_io);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "panel io init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Panel driver configuration
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BSP_LCD_RST_GPIO,
        .rgb_ele_order  = BSP_LCD_RGB_ORDER,
        .bits_per_pixel = 16,
    };
    ret = esp_lcd_new_panel_ili9341(panel_io, &panel_config, &panel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ili9341 panel init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_lcd_panel_reset(panel);
    esp_lcd_panel_init(panel);
    esp_lcd_panel_invert_color(panel, BSP_LCD_INVERT_COLOR);
    esp_lcd_panel_swap_xy(panel, BSP_LCD_SWAP_XY);
    esp_lcd_panel_mirror(panel, BSP_LCD_MIRROR_X, BSP_LCD_MIRROR_Y);

    // Turn on display
    esp_lcd_panel_disp_on_off(panel, true);

    ESP_LOGI(TAG, "LCD initialized: %dx%d, swap_xy=%d", BSP_LCD_HOR_RES, BSP_LCD_VER_RES, BSP_LCD_SWAP_XY);
    return ESP_OK;
}

esp_err_t bsp_lcd_fill_screen(uint16_t color)
{
    if (panel == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t *buffer = heap_caps_malloc(BSP_LCD_HOR_RES * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (buffer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    for (int i = 0; i < BSP_LCD_HOR_RES; i++) {
        buffer[i] = color;
    }

    for (int y = 0; y < BSP_LCD_VER_RES; y++) {
        esp_lcd_panel_draw_bitmap(panel, 0, y, BSP_LCD_HOR_RES, y + 1, buffer);
    }

    free(buffer);
    return ESP_OK;
}

esp_lcd_panel_handle_t bsp_lcd_get_panel(void)
{
    return panel;
}

esp_lcd_panel_io_handle_t bsp_lcd_get_panel_io(void)
{
    return panel_io;
}
```

- [ ] **Step 3: 更新 bsp_lcd.h 添加内部访问函数**

```c
#ifndef BSP_LCD_H
#define BSP_LCD_H

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_lcd_init(void);
esp_err_t bsp_lcd_fill_screen(uint16_t color);

// Internal accessors for lvgl_port
esp_lcd_panel_handle_t bsp_lcd_get_panel(void);
esp_lcd_panel_io_handle_t bsp_lcd_get_panel_io(void);

#ifdef __cplusplus
}
#endif

#endif // BSP_LCD_H
```

- [ ] **Step 4: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 5: Commit**

```bash
git add components/bsp/
git commit -m "feat(bsp): add ili9341 lcd driver"
```

---

### Task 1.3: 编写 LCD 点亮测试

**Files:**
- Modify: `main/main.c`

- [ ] **Step 1: 修改 main.c 添加 LCD 测试**

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_backlight.h"
#include "bsp_lcd.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-S3 Music Player starting");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    bsp_backlight_set(100);

    // Test: fill screen with red, green, blue
    while (1) {
        ESP_LOGI(TAG, "Fill red");
        bsp_lcd_fill_screen(0xF800); // RGB565 red
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Fill green");
        bsp_lcd_fill_screen(0x07E0); // RGB565 green
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Fill blue");
        bsp_lcd_fill_screen(0x001F); // RGB565 blue
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

- [ ] **Step 2: 修改 main/CMakeLists.txt 移除 music_player 依赖（暂时）**

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
git commit -m "feat(main): add lcd color test"
```

> **验证方式**：烧录到开发板，屏幕应循环显示红、绿、蓝三色，背光亮。

---

## 阶段 2：FT6336G 触摸驱动

### Task 2.1: 实现 FT6336G I2C 触摸读取

**Files:**
- Create: `components/bsp/include/bsp_touch.h`
- Create: `components/bsp/src/bsp_touch.c`
- Modify: `components/bsp/src/bsp_board.c`

- [ ] **Step 1: 创建 bsp_touch.h**

```c
#ifndef BSP_TOUCH_H
#define BSP_TOUCH_H

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_touch_init(void);
bool bsp_touch_read(uint16_t *x, uint16_t *y, bool *pressed);

#ifdef __cplusplus
}
#endif

#endif // BSP_TOUCH_H
```

- [ ] **Step 2: 创建 bsp_touch.c**

```c
#include "bsp_touch.h"
#include "bsp_pins.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "bsp_touch";

static i2c_master_bus_handle_t i2c_bus = NULL;
static i2c_master_dev_handle_t touch_dev = NULL;

esp_err_t bsp_touch_init(void)
{
    esp_err_t ret = ESP_OK;

    i2c_master_bus_config_t bus_cfg = {
        .i2c_port          = BSP_TOUCH_I2C_NUM,
        .sda_io_num        = BSP_TOUCH_I2C_SDA_GPIO,
        .scl_io_num        = BSP_TOUCH_I2C_SCL_GPIO,
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority     = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = 1,
        },
    };
    ret = i2c_new_master_bus(&bus_cfg, &i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    i2c_device_config_t dev_cfg = {
        .device_address = BSP_TOUCH_I2C_ADDR,
        .scl_speed_hz   = 400000,
        .scl_wait_us    = 0,
    };
    ret = i2c_master_bus_add_device(i2c_bus, &dev_cfg, &touch_dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "touch device add failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Reset touch controller
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BSP_TOUCH_RST_GPIO),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(BSP_TOUCH_RST_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(BSP_TOUCH_RST_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "touch initialized");
    return ESP_OK;
}

bool bsp_touch_read(uint16_t *x, uint16_t *y, bool *pressed)
{
    if (touch_dev == NULL) {
        return false;
    }

    uint8_t reg = 0x02;
    uint8_t buf[5] = {0};

    esp_err_t ret = i2c_master_transmit_receive(touch_dev, &reg, 1, buf, 5, 50);
    if (ret != ESP_OK) {
        return false;
    }

    uint8_t points = buf[0] & 0x0F;
    if (points == 0) {
        *pressed = false;
        return true;
    }

    *x = ((buf[1] & 0x0F) << 8) | buf[2];
    *y = ((buf[3] & 0x0F) << 8) | buf[4];
    *pressed = true;

    ESP_LOGD(TAG, "touch raw x=%u y=%u", *x, *y);
    return true;
}
```

- [ ] **Step 3: 修改 bsp_board.c 确保 I2C 只初始化一次**

由于触摸和音频共用 I2C 总线，后续音频任务会扩展 `bsp_board.c`。当前先保持简单，触摸初始化创建 I2C 总线即可。

- [ ] **Step 4: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 5: Commit**

```bash
git add components/bsp/
git commit -m "feat(bsp): add ft6336g touch driver"
```

---

### Task 2.2: 实现横屏触摸坐标映射

**Files:**
- Modify: `components/bsp/src/bsp_touch.c`
- Modify: `components/bsp/include/bsp_touch.h`

触摸面板物理坐标为 240×320（竖屏），项目使用横屏 320×240，需要将坐标旋转 90°。

旋转公式（顺时针 90°）：
- `rotated_x = y`
- `rotated_y = 239 - x`

- [ ] **Step 1: 修改 bsp_touch.h 添加坐标映射选项**

```c
#ifndef BSP_TOUCH_H
#define BSP_TOUCH_H

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_touch_init(void);
bool bsp_touch_read(uint16_t *x, uint16_t *y, bool *pressed);

// Coordinate mapping for landscape mode
void bsp_touch_map_to_landscape(uint16_t raw_x, uint16_t raw_y, uint16_t *lcd_x, uint16_t *lcd_y);

#ifdef __cplusplus
}
#endif

#endif // BSP_TOUCH_H
```

- [ ] **Step 2: 修改 bsp_touch.c 添加映射函数并更新读取函数**

```c
// Physical touch panel resolution (portrait)
#define TOUCH_PANEL_WIDTH   240
#define TOUCH_PANEL_HEIGHT  320

// LCD resolution after rotation (landscape)
#define LCD_WIDTH           BSP_LCD_HOR_RES
#define LCD_HEIGHT          BSP_LCD_VER_RES

void bsp_touch_map_to_landscape(uint16_t raw_x, uint16_t raw_y, uint16_t *lcd_x, uint16_t *lcd_y)
{
    // Clamp raw coordinates
    if (raw_x >= TOUCH_PANEL_WIDTH) raw_x = TOUCH_PANEL_WIDTH - 1;
    if (raw_y >= TOUCH_PANEL_HEIGHT) raw_y = TOUCH_PANEL_HEIGHT - 1;

    // Rotate 90 degrees clockwise: (x, y) -> (y, 239 - x)
    *lcd_x = (raw_y * LCD_WIDTH) / TOUCH_PANEL_HEIGHT;
    *lcd_y = LCD_HEIGHT - 1 - ((raw_x * LCD_HEIGHT) / TOUCH_PANEL_WIDTH);
}
```

修改 `bsp_touch_read` 函数，在读取后调用映射：

```c
bool bsp_touch_read(uint16_t *x, uint16_t *y, bool *pressed)
{
    // ... existing read code ...

    uint16_t raw_x = ((buf[1] & 0x0F) << 8) | buf[2];
    uint16_t raw_y = ((buf[3] & 0x0F) << 8) | buf[4];
    *pressed = true;

    bsp_touch_map_to_landscape(raw_x, raw_y, x, y);

    ESP_LOGD(TAG, "touch raw x=%u y=%u -> lcd x=%u y=%u", raw_x, raw_y, *x, *y);
    return true;
}
```

- [ ] **Step 3: 在 main.c 中添加触摸日志测试**

```c
#include "bsp_touch.h"

// In app_main, replace the color loop with:
while (1) {
    uint16_t x, y;
    bool pressed;
    if (bsp_touch_read(&x, &y, &pressed) && pressed) {
        ESP_LOGI(TAG, "touch: x=%u y=%u", x, y);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
}
```

- [ ] **Step 4: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 5: Commit**

```bash
git add components/bsp/ main/main.c
git commit -m "feat(bsp): add landscape touch coordinate mapping"
```

---

## 阶段 3：LVGL 移植与横屏

### Task 3.1: 添加 LVGL 组件依赖

**Files:**
- Create: `components/lvgl_port/CMakeLists.txt`
- Create: `components/lvgl_port/idf_component.yml`
- Modify: `main/CMakeLists.txt`

- [ ] **Step 1: 创建 lvgl_port 组件 CMakeLists.txt**

```cmake
idf_component_register(
    SRCS "src/lvgl_port.c"
    INCLUDE_DIRS "include"
    REQUIRES bsp lvgl esp_lvgl_port
)
```

- [ ] **Step 2: 创建 idf_component.yml 声明依赖**

```yaml
dependencies:
  lvgl/lvgl: "^9.0.0"
  espressif/esp_lvgl_port: "^2.0.0"
  espressif/esp_lcd_ili9341: "^1.0.0"
```

- [ ] **Step 3: 修改 main/CMakeLists.txt 添加 lvgl_port 依赖**

```cmake
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES bsp lvgl_port)
```

- [ ] **Step 4: 编译验证（会下载组件）**

```bash
idf.py build
```

Expected: 自动下载 lvgl 和 esp_lvgl_port 组件，编译成功。

- [ ] **Step 5: Commit**

```bash
git add components/lvgl_port/ main/CMakeLists.txt
git commit -m "feat(lvgl): add lvgl component dependencies"
```

---

### Task 3.2: 实现 LVGL 显示刷新接口

**Files:**
- Create: `components/lvgl_port/include/lvgl_port.h`
- Create: `components/lvgl_port/src/lvgl_port.c`

- [ ] **Step 1: 创建 lvgl_port.h**

```c
#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t lvgl_port_init(void);
void lvgl_port_test(void);

#ifdef __cplusplus
}
#endif

#endif // LVGL_PORT_H
```

- [ ] **Step 2: 创建 lvgl_port.c 显示移植**

```c
#include "lvgl_port.h"
#include "bsp_lcd.h"
#include "bsp_pins.h"
#include "bsp_backlight.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"

static const char *TAG = "lvgl_port";

static lv_display_t *display = NULL;

static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;

esp_err_t lvgl_port_init(void)
{
    ESP_LOGI(TAG, "Initialize LVGL");
    lv_init();

    lvgl_port_cfg_t port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    port_cfg.task_priority = 5;
    ESP_ERROR_CHECK(lvgl_port_init(&port_cfg));

    // Allocate display buffers
    size_t buffer_size = BSP_LCD_HOR_RES * 20;
    buf1 = heap_caps_malloc(buffer_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (buf1 == NULL) {
        return ESP_ERR_NO_MEM;
    }

    const lvgl_port_display_cfg_t display_cfg = {
        .io_handle   = bsp_lcd_get_panel_io(),
        .panel_handle = bsp_lcd_get_panel(),
        .buffer_size = buffer_size,
        .double_buffer = false,
        .hres        = BSP_LCD_HOR_RES,
        .vres        = BSP_LCD_VER_RES,
        .monochrome  = false,
        .rotation = {
            .swap_xy = BSP_LCD_SWAP_XY,
            .mirror_x = BSP_LCD_MIRROR_X,
            .mirror_y = BSP_LCD_MIRROR_Y,
        },
        .color_format = LV_COLOR_FORMAT_RGB565,
        .flags = {
            .buff_dma     = 1,
            .buff_spiram  = 0,
            .sw_rotate    = 0,
            .swap_bytes   = 1,
            .full_refresh = 0,
            .direct_mode  = 0,
        },
    };

    display = lvgl_port_add_disp(&display_cfg);
    if (display == NULL) {
        ESP_LOGE(TAG, "Failed to add display");
        return ESP_FAIL;
    }

    bsp_backlight_set(100);
    ESP_LOGI(TAG, "LVGL display initialized");
    return ESP_OK;
}

void lvgl_port_test(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a1a), 0);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello ES3C28P!");
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}
```

- [ ] **Step 3: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 4: Commit**

```bash
git add components/lvgl_port/
git commit -m "feat(lvgl): implement display flush interface"
```

---

### Task 3.3: 实现 LVGL 触摸输入接口

**Files:**
- Modify: `components/lvgl_port/src/lvgl_port.c`
- Modify: `components/lvgl_port/include/lvgl_port.h`

- [ ] **Step 1: 修改 lvgl_port.h 添加输入设备**

```c
#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t lvgl_port_init(void);
void lvgl_port_test(void);
void lvgl_port_touch_read(lv_indev_t *indev, lv_indev_data_t *data);

#ifdef __cplusplus
}
#endif

#endif // LVGL_PORT_H
```

- [ ] **Step 2: 修改 lvgl_port.c 注册触摸输入设备**

在 `lvgl_port_init` 末尾添加：

```c
    // Register touch input device
    lv_indev_t *indev = lv_indev_create();
    if (indev == NULL) {
        ESP_LOGE(TAG, "Failed to create input device");
        return ESP_FAIL;
    }
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lvgl_port_touch_read);
    lv_indev_set_display(indev, display);
```

添加读取回调函数：

```c
void lvgl_port_touch_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;

    uint16_t x, y;
    bool pressed;

    if (bsp_touch_read(&x, &y, &pressed) && pressed) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
```

- [ ] **Step 3: 修改 lvgl_port.c 顶部包含 bsp_touch.h**

```c
#include "bsp_touch.h"
```

- [ ] **Step 4: 修改 lvgl_port_test 添加可点击按钮**

```c
void lvgl_port_test(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a1a), 0);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello ES3C28P!");
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *btn = lv_button_create(scr);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click me");
    lv_obj_center(btn_label);

    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        ESP_LOGI(TAG, "Button clicked");
    }, LV_EVENT_CLICKED, NULL);
}
```

- [ ] **Step 5: 编译验证**

```bash
idf.py build
```

Expected: 编译成功。

- [ ] **Step 6: Commit**

```bash
git add components/lvgl_port/
git commit -m "feat(lvgl): add touch input device support"
```

---

### Task 3.4: 在 main 中运行 LVGL 测试

**Files:**
- Modify: `main/main.c`

- [ ] **Step 1: 修改 main.c**

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "lvgl_port.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-S3 Music Player starting");

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

    lvgl_port_test();

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
git commit -m "feat(main): run lvgl display and touch test"
```

> **阶段 3 交付验证**：烧录后屏幕显示深色背景、标题文字和居中按钮；点击按钮时串口输出 `Button clicked`。

---

## 阶段 0-3 完成检查清单

- [ ] BSP 组件创建并编译通过
- [ ] ILI9341 屏幕点亮，可填充颜色
- [ ] 背光 PWM 可调
- [ ] FT6336G 触摸读取正常，横屏坐标映射正确
- [ ] LVGL 移植完成，显示和触摸均可用
- [ ] 测试页面可点击，串口有反馈
- [ ] 所有代码已提交

---

## 进入下一阶段

阶段 0-3 完成后，开始执行 [阶段 4-6 详细计划：音频 + SD 卡 + MP3 解码](./2026-08-05-phase1-audio-sdcard-plan.md)。
