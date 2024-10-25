#include <lvgl.h>
#if LV_USE_BTN

#define BUTTON_SIZE_W 180
#define BUTTON_SIZE_H 100

static void event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("Clicked");
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_LOG_USER("Toggled");
    }
}

void lv_display_button(void)
{
    lv_obj_t *label;

    // Button 1
    lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -150);
    lv_obj_set_size(btn1, BUTTON_SIZE_W, BUTTON_SIZE_H);

    label = lv_label_create(btn1);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_label_set_text(label, "+");
    lv_obj_center(label);

    // Button 2
    lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 150);
    lv_obj_set_size(btn2, BUTTON_SIZE_W, BUTTON_SIZE_H);

    label = lv_label_create(btn2);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_label_set_text(label, "-");
    lv_obj_center(label);

    // Button 3
    lv_obj_t *btn3 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn3, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn3, LV_ALIGN_CENTER, 200, 0);
    lv_obj_add_flag(btn3, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn3, LV_SIZE_CONTENT);
    lv_obj_set_size(btn3, BUTTON_SIZE_W, BUTTON_SIZE_H);

    label = lv_label_create(btn3);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_40, 0);
    lv_label_set_text(label, "Switch");
    lv_obj_center(label);
}
#endif