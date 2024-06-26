#include <esp_event_base.h>

ESP_EVENT_DECLARE_BASE(BLUEWATCH_EVENTS);

typedef enum {
	BLUEWATCH_EVENT_WIFI_CONNECTING,
	BLUEWATCH_EVENT_DISABLE_SLEEP_CHANGE,
	BLUEWATCH_EVENT_BATTERY_UPDATE
} bluewatch_event_id_t;
