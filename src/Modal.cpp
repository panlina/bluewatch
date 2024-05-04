#include <lvgl.h>
#include "Modal.h"

Modal::Modal(bool borderless) {
	backdrop = lv_obj_create(lv_layer_top());
	lv_obj_set_size(backdrop, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_opa(backdrop, LV_OPA_TRANSP, LV_PART_MAIN);

	obj = lv_obj_create(lv_layer_top());
	lv_obj_set_align(obj, LV_ALIGN_CENTER);
	if (borderless) {
		lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
		lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
	}

	lv_obj_add_event_cb(backdrop, [](lv_event_t *e) {
		auto backdrop = lv_event_get_target(e);
		auto obj = (lv_obj_t *)lv_event_get_user_data(e);
		lv_obj_del_async(backdrop);
		lv_obj_del_async(obj);
	}, LV_EVENT_CLICKED, obj);
}

void Modal::close() {
	lv_obj_del(obj);
	lv_obj_del(backdrop);
}
