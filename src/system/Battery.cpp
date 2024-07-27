#include <LilyGoLib.h>
#include "event.h"
#include "../system.h"

class BatteryApp : public App {
	esp_event_handler_t batteryUpdateHandler, vbusInsertHandler, vbusRemoveHandler, batteryChargeDoneHandler, batteryChargeStartHandler;
public:
	void create(lv_obj_t *tile) {
		lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		auto batteryLabel = lv_label_create(tile);
		lv_label_set_text_fmt(batteryLabel, "%d%%", watch.getBatteryPercent());

		esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_UPDATE, batteryUpdateHandler = [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
			auto batteryLabel = (lv_obj_t *)event_handler_arg;
			lv_label_set_text_fmt(batteryLabel, "%d%%", watch.getBatteryPercent());
		}, batteryLabel);

		auto chargeLabel = lv_label_create(tile);
		lv_label_set_text(chargeLabel, "Charging");
		(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);

		esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_INSERT, vbusInsertHandler = [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
			auto chargeLabel = (lv_obj_t *)event_handler_arg;
			(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
		}, chargeLabel);
		esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_REMOVE, vbusRemoveHandler = [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
			auto chargeLabel = (lv_obj_t *)event_handler_arg;
			(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
		}, chargeLabel);
		esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_DONE, batteryChargeDoneHandler = [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
			auto chargeLabel = (lv_obj_t *)event_handler_arg;
			(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
		}, chargeLabel);
		esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_START, batteryChargeStartHandler = [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
			auto chargeLabel = (lv_obj_t *)event_handler_arg;
			(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
		}, chargeLabel);
	}
	void destroy() {
		esp_event_handler_unregister(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_UPDATE, batteryUpdateHandler);
		esp_event_handler_unregister(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_INSERT, vbusInsertHandler);
		esp_event_handler_unregister(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_REMOVE, vbusRemoveHandler);
		esp_event_handler_unregister(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_DONE, batteryChargeDoneHandler);
		esp_event_handler_unregister(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_START, batteryChargeStartHandler);
	}
} batteryApp;
