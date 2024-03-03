#include <LilyGoLib.h>
#include <LV_Helper.h>
#include <WiFi.h>
#include "watchface.h"

const uint32_t screenTimeout = 10000;
const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const int timezone = +8;
const char *wifiSsid = "(ssid)";
const char *wifiPassword = "(password)";

lv_obj_t *tileview, *watchfaceTile, *appsTile;

void setup()
{
	Serial.begin(115200);
	watch.begin();
	beginLvglHelper();

	tileview = lv_tileview_create(lv_scr_act());
	lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

	watchfaceTile = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR);
	setupWatchface();

	appsTile = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR);
	lv_obj_t *label = lv_label_create(appsTile);
	lv_label_set_text(label, "(Apps)");
	lv_obj_center(label);

	configTime(timezone * 3600, 0, ntpServer1, ntpServer2);

	WiFi.begin(wifiSsid, wifiPassword);
}

void loop()
{
	lv_task_handler();
	if (lv_disp_get_inactive_time(NULL) >= screenTimeout)
		enterLightSleep();

	watchfaceHandler();

	delay(5);
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
