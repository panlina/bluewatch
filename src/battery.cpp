#include <LilyGoLib.h>
#include "event.h"

unsigned long batteryLastUpdateTime;

void setupBattery() {
}

void batteryHandler() {
	auto now = millis();

	if (now - batteryLastUpdateTime >= 60 * 1000) {
		esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_UPDATE, nullptr, 0, 0);
		batteryLastUpdateTime = now;
	}
}
