#include <LilyGoLib.h>
#include "event.h"
#include "setting.h"

const uint32_t screenTimeout = 10000;
bool disableSleep;

void setDisableSleep(bool value) {
	disableSleep = value;
	if (!value)
		lv_disp_trig_activity(NULL);
	esp_event_post(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_DISABLE_SLEEP_CHANGE, nullptr, 0, 0);
	setting.set(".disableSleep", Json(disableSleep));
}

void setupSleep() {
	setDisableSleep((bool)setting.get(".disableSleep"));
}

void sleepHandler() {
	void enterLightSleep();
	if (!disableSleep)
		if (lv_disp_get_inactive_time(NULL) >= screenTimeout)
			enterLightSleep();
}

void enterLightSleep() {
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
