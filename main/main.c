#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_sdcard.h"

static const char *TAG = "MAIN";

static bool is_mp3(const char *filename)
{
    size_t len = strlen(filename);
    return len > 4 && strcasecmp(filename + len - 4, ".mp3") == 0;
}

void app_main(void)
{
    ESP_LOGI(TAG, "SD card file enumeration test");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    DIR *dir = opendir(bsp_sdcard_get_mount_point());
    if (dir == NULL) {
        ESP_LOGE(TAG, "failed to open dir");
        return;
    }

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (is_mp3(entry->d_name)) {
            ESP_LOGI(TAG, "MP3: %s", entry->d_name);
            count++;
        }
    }
    closedir(dir);

    ESP_LOGI(TAG, "found %d mp3 files", count);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
