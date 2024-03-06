#include <LilyGoLib.h>
#include "js.h"

static void createApp(lv_obj_t *parent, const char *name, const char *source);

void setupApps()
{
	extern lv_obj_t *appsTile;
	lv_obj_t *list = lv_obj_create(appsTile);
	lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));
	lv_obj_set_flex_flow(list, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);

	createApp(list, "print", "print('abc');");
	createApp(list, "vibrate", "vibrate();");
}

static void createApp(lv_obj_t *parent, const char *name, const char *source)
{
	lv_obj_t *btn = lv_btn_create(parent);
	lv_obj_set_size(btn, 100, 100);
	lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000ff), 0);

	lv_obj_t *label = lv_label_create(btn);
	lv_label_set_text(label, name);
	lv_obj_center(label);

	lv_obj_clear_flag(btn, LV_OBJ_FLAG_PRESS_LOCK);
	lv_obj_add_event_cb(
		btn,
		[](lv_event_t *e) {
			auto source = (const char *)lv_event_get_user_data(e);
			evalJs(source);
		},
		LV_EVENT_CLICKED,
		(void *)source
	);
}
