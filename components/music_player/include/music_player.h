#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化音乐播放器组件
 */
void music_player_init(void);

/**
 * @brief 播放指定索引的歌曲
 *
 * @param index 歌曲索引
 */
void music_player_play(uint32_t index);

/**
 * @brief 暂停播放
 */
void music_player_pause(void);

/**
 * @brief 继续播放
 */
void music_player_resume(void);

/**
 * @brief 播放下一首
 */
void music_player_next(void);

/**
 * @brief 播放上一首
 */
void music_player_prev(void);

/**
 * @brief 设置音量
 *
 * @param volume 音量值，范围 0-100
 */
void music_player_set_volume(uint8_t volume);

/**
 * @brief 获取当前播放状态
 *
 * @return true 正在播放，false 已暂停/停止
 */
bool music_player_is_playing(void);

#ifdef __cplusplus
}
#endif
