#include <WebServer.h>
#include "js.h"

WebServer webServer;

void setupWebServer()
{
	webServer.on("/eval", []() {
		auto code = webServer.arg("plain");
		try {
			auto result = evalJs(code.c_str());
			webServer.send(200, "text/plain", result);
		} catch (const char *error) {
			webServer.send(500, "text/plain", error);
		}
	});
	webServer.begin();
}
