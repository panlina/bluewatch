#include <LilyGoLib.h>
#include <LV_Helper.h>
#include <WiFi.h>

const uint32_t screenTimeout = 10000;
const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const int timezone = +8;
const char *wifiSsid = "(ssid)";
const char *wifiPassword = "(password)";

lv_obj_t *timeLabel, *dateLabel;

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

	configTime(timezone * 3600, 0, ntpServer1, ntpServer2);

	WiFi.begin(wifiSsid, wifiPassword);
}

void loop()
{
	lv_task_handler();
	if (lv_disp_get_inactive_time(NULL) >= screenTimeout)
		enterLightSleep();

	static unsigned long timeLastUpdateTime;
	auto now = millis();
	if (now - timeLastUpdateTime >= 1000) {
		updateTime();
		timeLastUpdateTime = now;
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
