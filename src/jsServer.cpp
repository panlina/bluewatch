#include <WebServer.h>
#include "js.h"

WebServer jsServer;

void setupJsServer()
{
	jsServer.on("/eval", []() {
		auto code = jsServer.arg("plain");
		try {
			auto result = evalJs(code.c_str());
			jsServer.send(200, "text/plain", result);
		} catch (const char *error) {
			jsServer.send(500, "text/plain", error);
		}
	});
	jsServer.begin();
}
