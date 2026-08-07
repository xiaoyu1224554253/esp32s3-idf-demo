#include "bsp_audio.h"
#include "bsp_pins.h"
#include "bsp_board.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "driver/i2s_std.h"
#include "esp_log.h"

static const char *TAG = "bsp_audio";

static i2s_chan_handle_t s_tx_chan = NULL;
static i2s_chan_handle_t s_rx_chan = NULL;
static esp_codec_dev_handle_t s_codec = NULL;

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
    ret = i2s_new_channel(&chan_cfg, &s_tx_chan, &s_rx_chan);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2s channel create failed: %s", esp_err_to_name(ret));
        return ret;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg = {
            .sample_rate_hz = BSP_AUDIO_SAMPLE_RATE,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256,
        },
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = BSP_AUDIO_MCLK_GPIO,
            .bclk = BSP_AUDIO_BCLK_GPIO,
            .ws   = BSP_AUDIO_LRCK_GPIO,
            .dout = BSP_AUDIO_DOUT_GPIO,
            .din  = BSP_AUDIO_DIN_GPIO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    ret = i2s_channel_init_std_mode(s_tx_chan, &std_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2s tx init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    i2s_std_clk_config_t rx_clk_cfg = {
        .sample_rate_hz = BSP_AUDIO_SAMPLE_RATE,
        .clk_src = I2S_CLK_SRC_DEFAULT,
        .mclk_multiple = I2S_MCLK_MULTIPLE_256,
    };
    i2s_std_slot_config_t rx_slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
        I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    i2s_std_gpio_config_t rx_gpio_cfg = {
        .mclk = BSP_AUDIO_MCLK_GPIO,
        .bclk = BSP_AUDIO_BCLK_GPIO,
        .ws   = BSP_AUDIO_LRCK_GPIO,
        .dout = BSP_AUDIO_DOUT_GPIO,
        .din  = BSP_AUDIO_DIN_GPIO,
    };
    ret = i2s_channel_init_std_mode(s_rx_chan, &(i2s_std_config_t){
        .clk_cfg = rx_clk_cfg,
        .slot_cfg = rx_slot_cfg,
        .gpio_cfg = rx_gpio_cfg,
    });
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2s rx init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    i2c_master_bus_handle_t i2c_bus = bsp_i2c_get_bus();
    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = i2c_bus,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
    };
    codec_ctrl_if_t *ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);

    gpio_if_t *gpio_if = audio_codec_new_gpio();

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
    s_codec = es8311_codec_new(&es8311_cfg);
    if (s_codec == NULL) {
        ESP_LOGE(TAG, "es8311 codec create failed");
        return ESP_FAIL;
    }

    esp_codec_dev_cfg_t dev_cfg = {
        .dev_type = MEDIA_HAL_CODEC_BOTH,
        .codec_if = s_codec,
        .codec_i2s_cfg = {
            .port = I2S_NUM_0,
            .role = I2S_ROLE_MASTER,
            .tx_handle = s_tx_chan,
            .rx_handle = s_rx_chan,
        },
    };
    s_codec = esp_codec_dev_new(&dev_cfg);
    if (s_codec == NULL) {
        ESP_LOGE(TAG, "codec device create failed");
        return ESP_FAIL;
    }

    ret = esp_codec_dev_open(s_codec, ESP_CODEC_DEV_TYPE_OUT | ESP_CODEC_DEV_TYPE_IN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "codec open failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_codec_dev_sample_info_t fs = {
        .sample_rate = BSP_AUDIO_SAMPLE_RATE,
        .channel = BSP_AUDIO_CHANNELS,
        .bits_per_sample = BSP_AUDIO_BITS,
    };
    esp_codec_dev_set_in_channel(s_codec, ESP_CODEC_DEV_MAKE_CHANNEL_MASK(0));
    esp_codec_dev_set_fs(s_codec, &fs, ESP_CODEC_DEV_TYPE_OUT);
    esp_codec_dev_set_fs(s_codec, &fs, ESP_CODEC_DEV_TYPE_IN);
    esp_codec_dev_mute(s_codec, true);

    ESP_LOGI(TAG, "audio initialized");
    return ESP_OK;
}

esp_err_t bsp_audio_start(void)
{
    if (s_codec == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret = esp_codec_dev_mute(s_codec, false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "audio unmute failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t bsp_audio_stop(void)
{
    if (s_codec == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_codec_dev_mute(s_codec, true);
}

esp_err_t bsp_audio_set_volume(uint8_t volume)
{
    if (s_codec == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (volume > 100) {
        volume = 100;
    }
    return esp_codec_dev_set_out_vol(s_codec, volume);
}

int bsp_audio_write(const int16_t *data, int samples)
{
    if (s_codec == NULL) {
        return -1;
    }
    int written = 0;
    esp_err_t ret = esp_codec_dev_write(s_codec, (void *)data, samples * sizeof(int16_t), &written, 1000);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "audio write failed: %s", esp_err_to_name(ret));
        return -1;
    }
    return written / sizeof(int16_t);
}
