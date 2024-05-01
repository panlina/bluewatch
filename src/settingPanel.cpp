#include <LilyGoLib.h>
#include <WiFi.h>
#include "event.h"
#include "setting.h"
#include "json.h"

lv_obj_t *settingPanel;
static lv_coord_t touchY0;
static lv_coord_t panelY0;
static bool isDragging;

static const lv_coord_t stickOut = 10;

extern String wifiSsid;
extern String wifiPassword;
extern bool wifiConnecting;

void setupSettingPanel() {
	settingPanel = lv_obj_create(lv_scr_act());
	lv_obj_set_size(settingPanel, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_pad_all(settingPanel, 20, LV_PART_MAIN);
	lv_obj_set_style_border_width(settingPanel, 0, LV_PART_MAIN);
	lv_obj_set_style_radius(settingPanel, 0, LV_PART_MAIN);
	lv_obj_set_style_bg_color(settingPanel, lv_color_hex(0x333333), LV_PART_MAIN);
	lv_obj_set_style_bg_opa(settingPanel, LV_OPA_70, LV_PART_MAIN);
	lv_obj_set_pos(settingPanel, 0, -240 + stickOut);
	lv_obj_set_style_opa(settingPanel, LV_OPA_0, LV_PART_MAIN);

	lv_obj_add_event_cb(settingPanel, [](lv_event_t *e) {
		auto indev = lv_event_get_indev(e);
		lv_point_t point;
		lv_indev_get_point(indev, &point);
		isDragging = true;
		touchY0 = point.y;
		panelY0 = lv_obj_get_y(settingPanel);
		lv_obj_set_style_opa(settingPanel, LV_OPA_100, LV_PART_MAIN);
	}, LV_EVENT_PRESSED, nullptr);

	lv_obj_add_event_cb(settingPanel, [](lv_event_t *e) {
		if (!isDragging) return;
		auto indev = lv_event_get_indev(e);
		lv_point_t point;
		lv_indev_get_point(indev, &point);
		auto y = panelY0 + (point.y - touchY0);
		if (y > 0) y = 0;
		lv_obj_set_y(settingPanel, y);
	}, LV_EVENT_PRESSING, nullptr);

	lv_obj_add_event_cb(settingPanel, [](lv_event_t *e) {
		isDragging = false;
		lv_anim_t a;
		lv_anim_init(&a);
		lv_anim_set_var(&a, settingPanel);
		lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
		lv_anim_set_time(&a, 200);
		auto y = lv_obj_get_y(settingPanel);
		lv_anim_set_values(&a, y, y > -120 ? 0 : -240 + stickOut);
		lv_anim_set_ready_cb(&a, [](lv_anim_t *a) {
			if (a->end_value < 0)
				lv_obj_set_style_opa(settingPanel, LV_OPA_0, LV_PART_MAIN);
		});
		lv_anim_start(&a);
	}, LV_EVENT_RELEASED, nullptr);

	lv_obj_set_style_text_color(settingPanel, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_flex_flow(settingPanel, LV_FLEX_FLOW_COLUMN);

	auto wifiBtn = lv_btn_create(settingPanel);
	lv_obj_set_width(wifiBtn, LV_PCT(100));
	// TODO: extract style
	lv_obj_set_style_bg_color(wifiBtn, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_bg_opa(wifiBtn, LV_OPA_20, LV_PART_MAIN);
	auto colorPrimary = lv_theme_get_color_primary(wifiBtn);
	lv_obj_set_style_bg_color(wifiBtn, colorPrimary, LV_PART_MAIN | LV_STATE_CHECKED);
	lv_obj_set_style_bg_opa(wifiBtn, LV_OPA_100, LV_PART_MAIN | LV_STATE_CHECKED);
	auto wifiLabel = lv_label_create(wifiBtn);
	lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI " Not Connected");

	WiFi.onEvent([=](arduino_event_id_t id, arduino_event_info_t info) {
		switch (id) {
			case ARDUINO_EVENT_WIFI_STA_CONNECTED:
			case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			case ARDUINO_EVENT_WIFI_STA_GOT_IP:
			case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
			case ARDUINO_EVENT_WIFI_STA_LOST_IP:
				if (WiFi.isConnected()) {
					lv_label_set_text_fmt(wifiLabel, "%s %s", LV_SYMBOL_WIFI, wifiSsid.c_str());
					lv_obj_add_state(wifiBtn, LV_STATE_CHECKED);
				} else {
					lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI " Not Connected");
					lv_obj_clear_state(wifiBtn, LV_STATE_CHECKED);
				}
				break;
		}
	});

	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_WIFI_CONNECTING, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		auto wifiLabel = (lv_obj_t *)event_handler_arg;
		lv_label_set_text_fmt(wifiLabel, LV_SYMBOL_WIFI " Connecting...");
	}, wifiLabel);

	lv_obj_add_event_cb(wifiBtn, [](lv_event_t *e) {
		if (wifiConnecting) return;
		auto wifiBtn = lv_event_get_target(e);
		if (WiFi.isConnected()) {
			setting.set(".wifi", Json(false));
			WiFi.disconnect();
		} else {
			setting.set(".wifi", Json(true));
			WiFi.begin(wifiSsid, wifiPassword);
			esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_WIFI_CONNECTING, nullptr, 0, 0);
		}
	}, LV_EVENT_CLICKED, nullptr);

	auto batteryBtn = lv_btn_create(settingPanel);
	lv_obj_set_width(batteryBtn, LV_PCT(100));
	lv_obj_set_style_bg_color(batteryBtn, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_bg_opa(batteryBtn, LV_OPA_20, LV_PART_MAIN);
	auto batteryLabel = lv_label_create(batteryBtn);
	lv_label_set_text_fmt(batteryLabel, "%s %d%%", LV_SYMBOL_BATTERY_FULL, watch.getBatteryPercent());
	// TODO: update battery periodically
}
