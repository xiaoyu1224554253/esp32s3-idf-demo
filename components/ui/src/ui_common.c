#include "ui_common.h"

void ui_set_style_text(lv_obj_t *obj, lv_color_t color, const lv_font_t *font)
{
    if (font) {
        lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(obj, color, LV_PART_MAIN);
}

void ui_set_style_bg(lv_obj_t *obj, lv_color_t color, lv_opa_t opa)
{
    lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, opa, LV_PART_MAIN);
}

void ui_set_style_border(lv_obj_t *obj, lv_color_t color, lv_coord_t width, lv_coord_t radius)
{
    lv_obj_set_style_border_color(obj, color, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, width, LV_PART_MAIN);
    lv_obj_set_style_border_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, radius, LV_PART_MAIN);
}

void ui_set_style_shadow(lv_obj_t *obj, lv_color_t color, lv_coord_t width, lv_opa_t opa)
{
    lv_obj_set_style_shadow_color(obj, color, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(obj, width, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(obj, opa, LV_PART_MAIN);
    lv_obj_set_style_shadow_spread(obj, 0, LV_PART_MAIN);
}

lv_obj_t *ui_create_round_btn(lv_obj_t *parent, lv_coord_t size, lv_color_t bg_color, const char *icon, const lv_font_t *font)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, size, size);
    lv_obj_set_style_radius(btn, size / 2, LV_PART_MAIN);
    ui_set_style_bg(btn, bg_color, LV_OPA_COVER);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, icon);
    ui_set_style_text(label, UI_COLOR_TEXT, font);
    lv_obj_center(label);

    return btn;
}
