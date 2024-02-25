#include <LilyGoLib.h>
#include <LV_Helper.h>

const uint32_t screenTimeout = 10000;

void setup()
{
	Serial.begin(115200);
	watch.begin();
	beginLvglHelper();

	lv_obj_t *label = lv_label_create(lv_scr_act());
	lv_obj_center(label);
	lv_label_set_text(label, "bluewatch");
}

void loop()
{
	lv_task_handler();
	if (lv_disp_get_inactive_time(NULL) >= screenTimeout)
		enterLightSleep();
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
