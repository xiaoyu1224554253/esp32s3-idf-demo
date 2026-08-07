#include "bsp_sdcard.h"
#include "bsp_pins.h"
#include "esp_vfs_fat.h"
#include "sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

static const char *TAG = "bsp_sdcard";

#define MOUNT_POINT "/sdcard"

static sdmmc_card_t *s_card = NULL;
static bool s_mounted = false;

esp_err_t bsp_sdcard_init(void)
{
    esp_err_t ret = ESP_OK;

    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_1;
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;

    sdmmc_slot_config_t slot_cfg = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_cfg.clk = BSP_SD_CLK_GPIO;
    slot_cfg.cmd = BSP_SD_CMD_GPIO;
    slot_cfg.d0 = BSP_SD_D0_GPIO;
    slot_cfg.d1 = BSP_SD_D1_GPIO;
    slot_cfg.d2 = BSP_SD_D2_GPIO;
    slot_cfg.d3 = BSP_SD_D3_GPIO;
    slot_cfg.width = 4;
    slot_cfg.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_cfg, &mount_cfg, &s_card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "sdcard mount failed: %s", esp_err_to_name(ret));
        return ret;
    }

    s_mounted = true;
    sdmmc_card_print_info(stdout, s_card);
    ESP_LOGI(TAG, "sdcard mounted at %s", MOUNT_POINT);
    return ESP_OK;
}

const char *bsp_sdcard_mount_point(void)
{
    return MOUNT_POINT;
}

bool bsp_sdcard_mounted(void)
{
    return s_mounted;
}
