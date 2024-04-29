#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "event.h"
#include "setting.h"

String wifiSsid;
String wifiPassword;

bool wifiConnecting;

void registerEventHandlers()
{
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_WIFI_CONNECTING, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		wifiConnecting = true;
	}, nullptr);

	WiFi.onEvent([=](arduino_event_id_t id, arduino_event_info_t info) {
		switch (id) {
			case ARDUINO_EVENT_WIFI_STA_CONNECTED:
			case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			case ARDUINO_EVENT_WIFI_STA_GOT_IP:
			case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
			case ARDUINO_EVENT_WIFI_STA_LOST_IP:
				wifiConnecting = false;
				break;
		}
	});
}

void setupWifi()
{
	registerEventHandlers();

	auto file = SPIFFS.open("/wifi.txt");
	if (file) {
		wifiSsid = file.readStringUntil('\n');
		wifiPassword = file.readStringUntil('\n');
		file.close();
		auto wifiEnabled = (bool)setting.get(".wifi");
		if (wifiEnabled) {
			WiFi.begin(wifiSsid, wifiPassword);
			esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_WIFI_CONNECTING, nullptr, 0, 0);
		}
	}
}
