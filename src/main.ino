#include <LilyGoLib.h>
#include <LV_Helper.h>
#include <WiFi.h>

const uint32_t screenTimeout = 10000;
const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const int timezone = +8;
const char *wifiSsid = "(ssid)";
const char *wifiPassword = "(password)";

lv_obj_t *timeLabel, *dateLabel, *statusBar, *batteryLabel, *wifiLabel;

unsigned long timeLastUpdateTime;
unsigned long batteryLastUpdateTime;

void setup()
{
	Serial.begin(115200);
	watch.begin();
	beginLvglHelper();

	timeLabel = lv_label_create(lv_scr_act());
	lv_obj_center(timeLabel);
	lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_48, LV_PART_MAIN);

	dateLabel = lv_label_create(lv_scr_act());
	lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 48);
	lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_24, LV_PART_MAIN);

	statusBar = lv_obj_create(lv_scr_act());
	lv_obj_set_width(statusBar, LV_PCT(100));
	lv_obj_set_height(statusBar, LV_SIZE_CONTENT);
	lv_obj_set_style_pad_all(statusBar, 4, LV_PART_MAIN);
	lv_obj_set_flex_flow(statusBar, LV_FLEX_FLOW_ROW_REVERSE);
	lv_obj_set_flex_align(statusBar, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_border_width(statusBar, 0, LV_PART_MAIN);
	lv_obj_set_style_text_font(statusBar, &lv_font_montserrat_24, LV_PART_MAIN);
	batteryLabel = lv_label_create(statusBar);
	wifiLabel = lv_label_create(statusBar);

	WiFi.onEvent([](arduino_event_t *e) {
		switch (e->event_id) {
			case ARDUINO_EVENT_WIFI_STA_CONNECTED:
			case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			case ARDUINO_EVENT_WIFI_STA_GOT_IP:
			case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
			case ARDUINO_EVENT_WIFI_STA_LOST_IP:
				updateWifiStatus();
		}
	});

	configTime(timezone * 3600, 0, ntpServer1, ntpServer2);

	WiFi.begin(wifiSsid, wifiPassword);

	auto now = millis();
	updateTime();
	timeLastUpdateTime = now;
	updateBatteryStatus();
	batteryLastUpdateTime = now;
	updateWifiStatus();
}

void loop()
{
	lv_task_handler();
	if (lv_disp_get_inactive_time(NULL) >= screenTimeout)
		enterLightSleep();

	auto now = millis();

	if (now - timeLastUpdateTime >= 1000) {
		updateTime();
		timeLastUpdateTime = now;
	}

	if (now - batteryLastUpdateTime >= 60 * 1000) {
		updateBatteryStatus();
		batteryLastUpdateTime = now;
	}

	delay(5);
}

void updateTime()
{
	tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		Serial.println("Failed to obtain time.");
		return;
	}
	char s[16];
	strftime(s, 16, "%X", &timeinfo);
	lv_label_set_text(timeLabel, s);
	strftime(s, 16, "%a %x", &timeinfo);
	lv_label_set_text(dateLabel, s);
}

void updateBatteryStatus()
{
	auto batteryPercentage = watch.getBatteryPercent();
	lv_label_set_text(batteryLabel,
		batteryPercentage > 90 ? LV_SYMBOL_BATTERY_FULL :
		batteryPercentage > 60 ? LV_SYMBOL_BATTERY_3 :
		batteryPercentage > 40 ? LV_SYMBOL_BATTERY_2 :
		batteryPercentage > 10 ? LV_SYMBOL_BATTERY_1 :
		LV_SYMBOL_BATTERY_EMPTY
	);
}

void updateWifiStatus()
{
	lv_label_set_text(wifiLabel, WiFi.isConnected() ? LV_SYMBOL_WIFI : "");
}

void enterLightSleep()
{
	Serial.println("Enter light sleep mode.");

	auto brightness = watch.getBrightness();
	watch.decrementBrightness(0);

	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
	gpio_wakeup_enable((gpio_num_t)BOARD_TOUCH_INT, GPIO_INTR_LOW_LEVEL);
	esp_sleep_enable_gpio_wakeup();

	esp_light_sleep_start();

	lv_disp_trig_activity(NULL);

	watch.incrementalBrightness(brightness);
}
