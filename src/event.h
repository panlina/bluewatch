#include <esp_event_base.h>

ESP_EVENT_DECLARE_BASE(BLUEWATCH_EVENTS);

typedef enum {
	BLUEWATCH_EVENT_WIFI_CONNECTING
} bluewatch_event_id_t;
