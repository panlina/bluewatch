#include <LilyGoLib.h>

void setupApps()
{
	extern lv_obj_t *appsTile;
	lv_obj_t *label = lv_label_create(appsTile);
	lv_label_set_text(label, "(Apps)");
	lv_obj_center(label);
}
