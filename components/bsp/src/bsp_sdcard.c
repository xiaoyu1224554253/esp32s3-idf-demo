#include "bsp_sdcard.h"
#include "bsp_pins.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include <stdbool.h>
#include <stdio.h>

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
    slot_config.clk = BSP_SDMMC_CLK_GPIO;
    slot_config.cmd = BSP_SDMMC_CMD_GPIO;
    slot_config.d0 = BSP_SDMMC_D0_GPIO;
    slot_config.d1 = BSP_SDMMC_D1_GPIO;
    slot_config.d2 = BSP_SDMMC_D2_GPIO;
    slot_config.d3 = BSP_SDMMC_D3_GPIO;
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
