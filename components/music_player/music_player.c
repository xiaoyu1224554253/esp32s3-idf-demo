#include "music_player.h"

#include "esp_log.h"

static const char *TAG = "music_player";

static bool s_is_playing = false;
static uint8_t s_volume = 50;
static uint32_t s_current_index = 0;

void music_player_init(void)
{
    s_is_playing = false;
    s_volume = 50;
    s_current_index = 0;
    ESP_LOGI(TAG, "music player initialized");
}

void music_player_play(uint32_t index)
{
    s_current_index = index;
    s_is_playing = true;
    ESP_LOGI(TAG, "play track %lu", index);
}

void music_player_pause(void)
{
    s_is_playing = false;
    ESP_LOGI(TAG, "paused");
}

void music_player_resume(void)
{
    s_is_playing = true;
    ESP_LOGI(TAG, "resumed");
}

void music_player_next(void)
{
    s_current_index++;
    s_is_playing = true;
    ESP_LOGI(TAG, "next track, index %lu", s_current_index);
}

void music_player_prev(void)
{
    if (s_current_index > 0) {
        s_current_index--;
    }
    s_is_playing = true;
    ESP_LOGI(TAG, "previous track, index %lu", s_current_index);
}

void music_player_set_volume(uint8_t volume)
{
    if (volume > 100) {
        volume = 100;
    }
    s_volume = volume;
    ESP_LOGI(TAG, "volume set to %u", s_volume);
}

bool music_player_is_playing(void)
{
    return s_is_playing;
}
