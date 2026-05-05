#include "fallback_ui.h"

#include "lvgl.h"

void fallback_ui_load(void)
{
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, 800, 480);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x111827), 0);

    lv_obj_t *bar = lv_obj_create(screen);
    lv_obj_set_size(bar, 800, 48);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x1F2937), 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_radius(bar, 0, 0);

    lv_obj_t *title = lv_label_create(bar);
    lv_label_set_text(title, "CAN Panel 4.3");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);

    lv_obj_t *status = lv_label_create(screen);
    lv_label_set_text(status, "Fallback UI loaded - generated LVGL XML output not present");
    lv_obj_set_style_text_color(status, lv_color_hex(0xE5E7EB), 0);
    lv_obj_align(status, LV_ALIGN_CENTER, 0, -48);

    lv_obj_t *button = lv_button_create(screen);
    lv_obj_set_size(button, 200, 80);
    lv_obj_align(button, LV_ALIGN_CENTER, 0, 48);

    lv_obj_t *button_label = lv_label_create(button);
    lv_label_set_text(button_label, "Bring-up OK");
    lv_obj_center(button_label);

    lv_screen_load(screen);
}
