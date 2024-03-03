#include <LilyGoLib.h>
#include "watchface.h"
#include "apps.h"

lv_obj_t *tileview, *watchfaceTile, *appsTile;

void setupUi()
{
	tileview = lv_tileview_create(lv_scr_act());
	lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

	watchfaceTile = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR);
	setupWatchface();

	appsTile = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR);
	setupApps();
}
