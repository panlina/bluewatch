#include <Arduino.h>
#include <duktape.h>
#include <WiFi.h>

static duk_ret_t js_gatewayIP(duk_context *ctx) {
	auto ip = WiFi.gatewayIP();
	duk_push_string(ctx, ip.toString().c_str());
	return 1;
}

static duk_ret_t js_localIP(duk_context *ctx) {
	auto ip = WiFi.localIP();
	duk_push_string(ctx, ip.toString().c_str());
	return 1;
}

void duktape_watch_install_wifi(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_gatewayIP, 0);
	duk_put_prop_string(ctx, -2, "gatewayIP");
	duk_push_c_function(ctx, js_localIP, 0);
	duk_put_prop_string(ctx, -2, "localIP");
	duk_put_global_string(ctx, "wifi");
}
