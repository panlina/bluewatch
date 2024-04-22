#include <LilyGoLib.h>
#include <duktape.h>
#include <duktape_lvgl.h>
#include <DuktapeWatch.h>
#include <SPIFFS.h>

duk_context *jsContext;

void setupJs()
{
	jsContext = duk_create_heap_default();
	duktape_lvgl_install(jsContext);
	duktape_watch_install(jsContext);
	void loadLibraries();
	loadLibraries();
}

void loadLibraries()
{
	auto dir = SPIFFS.open("/library");
	while (auto file = dir.openNextFile()) {
		auto source = file.readString();
		duk_push_string(jsContext, source.c_str());
		auto rc = duk_peval(jsContext);
		duk_pop(jsContext);
	}
}

const char *evalJs(const char *code)
{
	duk_push_string(jsContext, code);
	auto rc = duk_peval(jsContext);

	if (rc)
		duk_safe_to_stacktrace(jsContext, -1);
	else
		duk_safe_to_string(jsContext, -1);

	auto result = duk_get_string(jsContext, -1);

	duk_pop(jsContext);
	if (rc)
		throw result;
	else
		return result;
}
