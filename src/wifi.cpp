#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "event.h"

String wifiSsid;
String wifiPassword;

void setupWifi()
{
	auto file = SPIFFS.open("/wifi.txt");
	if (file) {
		wifiSsid = file.readStringUntil('\n');
		wifiPassword = file.readStringUntil('\n');
		file.close();
		WiFi.begin(wifiSsid, wifiPassword);
		esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_WIFI_CONNECTING, nullptr, 0, 0);
	}
}
