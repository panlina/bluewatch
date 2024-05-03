/*
This file name has a `_` suffix. It is used to avoid conflict with `<WiFi.h>`
*/

#include <Arduino.h>
#include <vector>

struct WifiNetwork {
	String ssid;
	String password;
};

struct WifiSetting {
	String network;
	bool enabled;
};

extern std::vector<WifiNetwork> wifiNetwork;
extern WifiSetting wifiSetting;
extern bool wifiConnecting;
void setupWifi();
