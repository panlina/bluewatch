#include <LilyGoLib.h>
#include "system.h"

extern App wifiApp, batteryApp;

extern lv_obj_t *systemTile;

void setupSystem()
{
	auto list = lv_list_create(systemTile);
	lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));

	void enterApp(App *app);

	auto wifiBtn = lv_list_add_btn(list, LV_SYMBOL_WIFI, "WiFi");
	lv_obj_add_event_cb(wifiBtn, [](lv_event_t *e) { enterApp(&wifiApp); }, LV_EVENT_CLICKED, nullptr);

	auto batteryBtn = lv_list_add_btn(list, LV_SYMBOL_BATTERY_FULL, "Battery");
	lv_obj_add_event_cb(batteryBtn, [](lv_event_t *e) { enterApp(&batteryApp); }, LV_EVENT_CLICKED, nullptr);
}

extern lv_obj_t *tileview, *systemTile;
static lv_obj_t *appTile;
App *app;
static void enterAppTile();

void enterApp(App *app) {
	::app = app;
	enterAppTile();
	try { app->create(appTile); }
	catch (const std::exception &e) {
		auto label = lv_label_create(appTile);
		lv_obj_set_align(label, LV_ALIGN_CENTER);
		lv_label_set_text(label, e.what());
	}
}

static void enterAppTile() {
	appTile = lv_tileview_add_tile(tileview, 1, 1, LV_DIR_HOR);
	lv_obj_set_tile_id(tileview, 1, 1, LV_ANIM_ON);

	lv_obj_add_event_cb(tileview, [](lv_event_t *e) {
		lv_obj_remove_event_cb_with_user_data(tileview, nullptr, appTile);
		lv_obj_add_event_cb(tileview, [](lv_event_t *e) {
			if (lv_tileview_get_tile_act(tileview) != systemTile) return;
			lv_obj_remove_event_cb_with_user_data(tileview, nullptr, systemTile);
			app->destroy();
			lv_obj_del(appTile);
			appTile = nullptr;
		}, LV_EVENT_SCROLL_END, systemTile);
	}, LV_EVENT_SCROLL_END, appTile);
}
