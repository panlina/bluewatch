#include "../system.h"

class BatteryApp : public App {
public:
	void create(lv_obj_t *tile) {
		auto label = lv_label_create(tile);
		lv_obj_set_align(label, LV_ALIGN_CENTER);
		lv_label_set_text(label, "Battery");
	}
	void destroy() {}
} batteryApp;
