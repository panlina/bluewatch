#include <WiFi.h>
#include "../system.h"

class WifiApp : public App {
public:
	void create(lv_obj_t *tile) {
		auto list = lv_list_create(tile);
		lv_obj_set_size(list, LV_PCT(100), LV_SIZE_CONTENT);

		auto nameBtn = lv_list_add_btn(list, nullptr, "Name");
		auto nameLabel = lv_label_create(nameBtn);
		lv_label_set_text(nameLabel, WiFi.SSID().c_str());

		auto localIPBtn = lv_list_add_btn(list, nullptr, "Local IP");
		auto localIPLabel = lv_label_create(localIPBtn);
		lv_label_set_text(localIPLabel, WiFi.localIP().toString().c_str());

		auto gatewayIPBtn = lv_list_add_btn(list, nullptr, "Gateway IP");
		auto gatewayIPLabel = lv_label_create(gatewayIPBtn);
		lv_label_set_text(gatewayIPLabel, WiFi.gatewayIP().toString().c_str());

		auto dnsIPBtn = lv_list_add_btn(list, nullptr, "DNS IP");
		auto dnsIPLabel = lv_label_create(dnsIPBtn);
		lv_label_set_text(dnsIPLabel, WiFi.dnsIP().toString().c_str());

		auto subnetMaskBtn = lv_list_add_btn(list, nullptr, "Subnet Mask");
		auto subnetMaskLabel = lv_label_create(subnetMaskBtn);
		lv_label_set_text(subnetMaskLabel, WiFi.subnetMask().toString().c_str());

		auto rssiBtn = lv_list_add_btn(list, nullptr, "RSSI");
		auto rssiLabel = lv_label_create(rssiBtn);
		lv_label_set_text(rssiLabel, String(WiFi.RSSI()).c_str());
	}
	void destroy() {}
} wifiApp;
