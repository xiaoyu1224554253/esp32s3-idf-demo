#ifndef UI_COMMON_H
#define UI_COMMON_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Screen layout */
#define UI_SCREEN_WIDTH         320
#define UI_SCREEN_HEIGHT        240
#define UI_STATUS_BAR_HEIGHT    18
#define UI_NAV_BAR_HEIGHT       30
#define UI_CONTENT_Y            UI_STATUS_BAR_HEIGHT
#define UI_CONTENT_HEIGHT       (UI_SCREEN_HEIGHT - UI_STATUS_BAR_HEIGHT - UI_NAV_BAR_HEIGHT)

/* Colors (LVGL 16-bit RGB565-ish, use full color macros) */
#define UI_COLOR_BG             lv_color_hex(0x0D0D15)
#define UI_COLOR_SURFACE        lv_color_hex(0x151520)
#define UI_COLOR_SURFACE_LIGHT  lv_color_hex(0x22222E)
#define UI_COLOR_TEXT           lv_color_hex(0xFFFFFF)
#define UI_COLOR_TEXT_SECONDARY lv_color_hex(0xAAAAAA)
#define UI_COLOR_TEXT_MUTED     lv_color_hex(0x666666)
#define UI_COLOR_BORDER         lv_color_hex(0x2A2A3E)
#define UI_COLOR_PRIMARY        lv_color_hex(0x7C6CFF)
#define UI_COLOR_PRIMARY_DARK   lv_color_hex(0x5A4FD4)
#define UI_COLOR_ACCENT_PURPLE  lv_color_hex(0xA855F7)
#define UI_COLOR_LIVE_RED       lv_color_hex(0xFF4757)

/* Chinese fonts generated from WQY MicroHei */
LV_FONT_DECLARE(font_cn_12);
LV_FONT_DECLARE(font_cn_14);

#define UI_FONT_SMALL           (&font_cn_12)
#define UI_FONT_NORMAL          (&font_cn_14)

/* Page indices */
typedef enum {
    UI_PAGE_PLAYER = 0,
    UI_PAGE_PLAYLIST,
    UI_PAGE_RADIO,
    UI_PAGE_SEARCH,
    UI_PAGE_COUNT
} ui_page_t;

/* Playback modes */
typedef enum {
    UI_PLAY_MODE_SEQUENCE = 0,
    UI_PLAY_MODE_RANDOM,
    UI_PLAY_MODE_REPEAT,
    UI_PLAY_MODE_COUNT
} ui_play_mode_t;

/* Shared helpers */
void ui_set_style_text(lv_obj_t *obj, lv_color_t color, const lv_font_t *font);
void ui_set_style_bg(lv_obj_t *obj, lv_color_t color, lv_opa_t opa);
void ui_set_style_border(lv_obj_t *obj, lv_color_t color, lv_coord_t width, lv_coord_t radius);
void ui_set_style_shadow(lv_obj_t *obj, lv_color_t color, lv_coord_t width, lv_opa_t opa);
lv_obj_t *ui_create_round_btn(lv_obj_t *parent, lv_coord_t size, lv_color_t bg_color, const char *icon, const lv_font_t *font);

#ifdef __cplusplus
}
#endif

#endif /* UI_COMMON_H */
