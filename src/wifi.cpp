#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>

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
	}
}
