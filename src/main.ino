#include <LilyGoLib.h>
#include <LV_Helper.h>

void setup()
{
	Serial.begin(115200);
	watch.begin();
	beginLvglHelper();

	lv_obj_t *label = lv_label_create(lv_scr_act());
	lv_obj_center(label);
	lv_label_set_text(label, "bluewatch");
}

void loop()
{
	lv_task_handler();
	delay(5);
}
