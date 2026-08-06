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
