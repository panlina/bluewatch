#include <LilyGoLib.h>
#include "event.h"

unsigned long batteryLastUpdateTime;

bool pmuFlag;

void setupBattery() {
	watch.attachPMU([]() { pmuFlag = true; });
	watch.clearPMU();
	watch.enableIRQ(
		XPOWERS_AXP2101_VBUS_INSERT_IRQ | XPOWERS_AXP2101_VBUS_REMOVE_IRQ |
		XPOWERS_AXP2101_BAT_CHG_DONE_IRQ | XPOWERS_AXP2101_BAT_CHG_START_IRQ
	);
}

void batteryHandler() {
	auto now = millis();

	if (now - batteryLastUpdateTime >= 60 * 1000) {
		esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_UPDATE, nullptr, 0, 0);
		batteryLastUpdateTime = now;
	}

	if (pmuFlag) {
		pmuFlag = false;
		watch.readPMU();
		if (watch.isVbusInsertIrq())
			esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_INSERT, nullptr, 0, 0);
		if (watch.isVbusRemoveIrq())
			esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_REMOVE, nullptr, 0, 0);
		if (watch.isBatChagerDoneIrq())
			esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_DONE, nullptr, 0, 0);
		if (watch.isBatChagerStartIrq())
			esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_START, nullptr, 0, 0);
		watch.clearPMU();
	}
}
