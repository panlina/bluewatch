#include <lvgl.h>

struct Modal {
	lv_obj_t *obj;
	lv_obj_t *backdrop;
	Modal(bool borderless = false);
	void close();
};
