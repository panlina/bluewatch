#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "event.h"
#include "setting.h"
#include "wifi_.h"
#include <vector>

using std::vector;

vector<WifiNetwork> wifiNetwork;
WifiSetting wifiSetting;

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

	if (SPIFFS.exists("/wifi-network.json")) {
		auto network = wifiNetworkSetting.get("");
		for (auto i = 0; i < network.length(); i++) {
			auto n = network[i];
			wifiNetwork.push_back(
				WifiNetwork{
					.ssid = (String)n["ssid"],
					.password = (String)n["password"]
				}
			);
		}
	}

	auto wifi = setting.get(".wifi");
	wifiSetting.network = (String)wifi["network"];
	wifiSetting.enabled = (bool)wifi["enabled"];
	if (wifiSetting.enabled) {
		auto network = std::find_if(wifiNetwork.begin(), wifiNetwork.end(), [=](const WifiNetwork &network) { return network.ssid == wifiSetting.network; });
		WiFi.begin(network->ssid, network->password);
		esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_WIFI_CONNECTING, nullptr, 0, 0);
	}
}
