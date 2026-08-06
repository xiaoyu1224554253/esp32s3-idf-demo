#include "mp3_decoder.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char *TAG = "mp3_decoder";

struct mp3_decoder {
    mp3dec_t mp3d;
    FILE *file;
    uint8_t *file_buffer;
    size_t file_size;
    size_t decode_pos;
};

mp3_decoder_t *mp3_decoder_create(void)
{
    mp3_decoder_t *dec = calloc(1, sizeof(mp3_decoder_t));
    if (dec == NULL) return NULL;
    mp3dec_init(&dec->mp3d);
    return dec;
}

void mp3_decoder_destroy(mp3_decoder_t *dec)
{
    if (dec == NULL) return;
    mp3_decoder_close(dec);
    free(dec);
}

bool mp3_decoder_open_file(mp3_decoder_t *dec, const char *path)
{
    if (dec == NULL || path == NULL) return false;

    mp3_decoder_close(dec);

    dec->file = fopen(path, "rb");
    if (dec->file == NULL) {
        ESP_LOGE(TAG, "failed to open %s", path);
        return false;
    }

    fseek(dec->file, 0, SEEK_END);
    dec->file_size = ftell(dec->file);
    fseek(dec->file, 0, SEEK_SET);

    dec->file_buffer = malloc(dec->file_size);
    if (dec->file_buffer == NULL) {
        fclose(dec->file);
        dec->file = NULL;
        return false;
    }

    size_t read = fread(dec->file_buffer, 1, dec->file_size, dec->file);
    if (read != dec->file_size) {
        ESP_LOGW(TAG, "only read %zu of %zu bytes", read, dec->file_size);
    }

    dec->decode_pos = 0;
    ESP_LOGI(TAG, "opened %s, size %zu", path, dec->file_size);
    return true;
}

void mp3_decoder_close(mp3_decoder_t *dec)
{
    if (dec == NULL) return;
    if (dec->file) {
        fclose(dec->file);
        dec->file = NULL;
    }
    if (dec->file_buffer) {
        free(dec->file_buffer);
        dec->file_buffer = NULL;
    }
    dec->file_size = 0;
    dec->decode_pos = 0;
}

int mp3_decoder_decode_frame(mp3_decoder_t *dec, int16_t *pcm, int pcm_size, mp3_info_t *info)
{
    if (dec == NULL || pcm == NULL) return 0;
    if (dec->file_buffer == NULL || dec->decode_pos >= dec->file_size) return 0;

    mp3dec_frame_info_t frame_info;
    int samples = mp3dec_decode_frame(&dec->mp3d,
                                       dec->file_buffer + dec->decode_pos,
                                       dec->file_size - dec->decode_pos,
                                       pcm, &frame_info);

    if (info != NULL) {
        info->sample_rate = frame_info.hz;
        info->channels = frame_info.channels;
        info->bitrate_kbps = frame_info.bitrate_kbps;
    }

    if (samples > 0) {
        dec->decode_pos += frame_info.frame_bytes;
    } else {
        dec->decode_pos++;
    }

    return samples;
}
