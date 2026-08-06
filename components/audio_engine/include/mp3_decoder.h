#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int sample_rate;
    int channels;
    int bitrate_kbps;
} mp3_info_t;

typedef struct mp3_decoder mp3_decoder_t;

mp3_decoder_t *mp3_decoder_create(void);
void mp3_decoder_destroy(mp3_decoder_t *dec);
bool mp3_decoder_open_file(mp3_decoder_t *dec, const char *path);
void mp3_decoder_close(mp3_decoder_t *dec);
int mp3_decoder_decode_frame(mp3_decoder_t *dec, int16_t *pcm, int pcm_size, mp3_info_t *info);

#ifdef __cplusplus
}
#endif

#endif // MP3_DECODER_H
