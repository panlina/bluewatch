#include <WebServer.h>
#include "js.h"
#include "SPIFFS.h"

WebServer webServer;

class PrefixUri: public Uri {
public:
	PrefixUri(const char *uri) : Uri(uri) {}
	PrefixUri(const String &uri) : Uri(uri) {}
	PrefixUri(const __FlashStringHelper *uri) : Uri((const char *)uri) {} 

	virtual Uri* clone() const {
		return new PrefixUri(_uri);
	};

	virtual bool canHandle(const String &requestUri, __attribute__((unused)) std::vector<String> &pathArgs) {
		return requestUri.startsWith(_uri);
	}
};

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
	webServer.on(PrefixUri("/fs"), HTTP_GET, []() {
		auto path = webServer.uri().substring(sizeof("/fs") - 1);
		auto file = SPIFFS.open(path);
		webServer.streamFile(file, "");
		file.close();
	});
	webServer.on(PrefixUri("/fs"), HTTP_PUT, []() {
		auto path = webServer.uri().substring(sizeof("/fs") - 1);
		auto body = webServer.arg("plain");
		auto file = SPIFFS.open(path, "w");
		file.write((uint8_t *)body.c_str(), body.length());
		file.close();
		webServer.send(204, nullptr, "");
	});
	webServer.on(PrefixUri("/fs"), HTTP_DELETE, []() {
		auto path = webServer.uri().substring(sizeof("/fs") - 1);
		SPIFFS.remove(path);
		webServer.send(204, nullptr, "");
	});
	webServer.begin();
}
