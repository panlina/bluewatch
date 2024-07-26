#include <LilyGoLib.h>
#include "watchface.h"
#include "system.h"
#include "apps.h"
#include "settingPanel.h"

lv_obj_t *tileview, *watchfaceTile, *systemTile, *appsTile;

void setupUi()
{
	tileview = lv_tileview_create(lv_scr_act());
	lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

	watchfaceTile = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_ALL);
	setupWatchface();

	systemTile = lv_tileview_add_tile(tileview, 0, 1, LV_DIR_VER);
	setupSystem();

	appsTile = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR);
	setupApps();

	setupSettingPanel();
}
