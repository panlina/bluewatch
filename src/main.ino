#include <LilyGoLib.h>
#include <LV_Helper.h>
#include "ui.h"
#include "sleep.h"
#include "battery.h"
#include "wifi_.h"
#include "js.h"
#include "webServer_.h"

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const int timezone = +8;

SET_LOOP_TASK_STACK_SIZE(16 * 1024);	// Duktape compile may use up default (8K) stack space

void setup()
{
	Serial.begin(115200);
	watch.begin();
	beginLvglHelper();
	esp_event_loop_create_default();

	void registerLvglFontDriver();
	registerLvglFontDriver();

	setupUi();
	setupSleep();
	setupBattery();

	configTime(timezone * 3600, 0, ntpServer1, ntpServer2);

	setupWifi();
	watch.configAccelerometer();
	watch.enableAccelerometer();

	setupJs();
	setupWebServer();
}

void loop()
{
	lv_task_handler();
	sleepHandler();
	batteryHandler();
	watchfaceHandler();
	webServer.handleClient();

	delay(5);
}
