#include <LilyGoLib.h>
#include "js.h"
#include <SPIFFS.h>

extern lv_obj_t *appsTile;
static lv_obj_t *list;
static void loadApps();

void setupApps()
{
	list = lv_obj_create(appsTile);
	lv_obj_set_style_border_side(list, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
	lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
	lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));
	lv_obj_set_flex_flow(list, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);
	loadApps();
}

static void createApp(const char *name);

static void loadApps()
{
	auto dir = SPIFFS.open("/app");
	while (auto file = dir.openNextFile()) {
		String name = file.name();
		createApp(name.substring(0, name.length() - 3).c_str());	// remove ".js"
	}
}

static void createApp(const char *name)
{
	lv_obj_t *btn = lv_btn_create(list);
	lv_obj_set_size(btn, 100, 100);
	lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000ff), 0);

	lv_obj_t *label = lv_label_create(btn);
	lv_label_set_text(label, name);
	lv_obj_center(label);

	lv_obj_clear_flag(btn, LV_OBJ_FLAG_PRESS_LOCK);
	lv_obj_add_event_cb(
		btn,
		[](lv_event_t *e) {
			auto name = (String *)lv_event_get_user_data(e);
			auto file = SPIFFS.open("/app/" + *name + ".js");
			auto source = file.readString();
			file.close();
			void enterApp(const char *source);
			enterApp(source.c_str());
		},
		LV_EVENT_CLICKED,
		new String(name)
	);
}

extern lv_obj_t *tileview, *appsTile;
static lv_obj_t *appTile;
static void enterAppTile();
static void (*onLeaveAppTile)();

void enterApp(const char *source) {
	onLeaveAppTile = []() {
		duk_push_global_stash(jsContext);
		if (duk_get_prop_string(jsContext, -1, "app_cleanup")) {
			duk_call(jsContext, 0);
			duk_pop(jsContext);
			duk_del_prop_string(jsContext, -1, "app_cleanup");
			duk_pop(jsContext);
		}
	};
	enterAppTile();
	duk_push_pointer(jsContext, appTile);
	duk_put_global_string(jsContext, "appTile");
	auto s = "(function(){" + String(source) + "})()";
	auto rc = duk_peval_string(jsContext, s.c_str());
	if (rc) {
		duk_safe_to_stacktrace(jsContext, -1);
		auto stacktrace = duk_get_string(jsContext, -1);
		auto label = lv_label_create(appTile);
		lv_obj_set_align(label, LV_ALIGN_CENTER);
		lv_label_set_text(label, stacktrace);
	} else
		if (duk_is_function(jsContext, -1)) {
			duk_push_global_stash(jsContext);
			duk_dup(jsContext, -2);
			duk_put_prop_string(jsContext, -2, "app_cleanup");
			duk_pop(jsContext);
		}
	duk_pop(jsContext);
}

static void enterAppTile() {
	appTile = lv_tileview_add_tile(tileview, 2, 0, LV_DIR_HOR);
	lv_obj_set_tile_id(tileview, 2, 0, LV_ANIM_ON);

	lv_obj_add_event_cb(tileview, [](lv_event_t *e) {
		lv_obj_remove_event_cb_with_user_data(tileview, nullptr, appTile);
		lv_obj_add_event_cb(tileview, [](lv_event_t *e) {
			if (lv_tileview_get_tile_act(tileview) != appsTile) return;
			lv_obj_remove_event_cb_with_user_data(tileview, nullptr, appsTile);
			(*onLeaveAppTile)();
			lv_obj_del(appTile);
			appTile = nullptr;
		}, LV_EVENT_SCROLL_END, appsTile);
	}, LV_EVENT_SCROLL_END, appTile);
}
