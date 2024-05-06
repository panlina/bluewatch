#include <LilyGoLib.h>
#include <duktape.h>

static duk_ret_t js_print(duk_context *ctx) {
	duk_push_string(ctx, " ");
	duk_insert(ctx, 0);
	duk_join(ctx, duk_get_top(ctx) - 1);
	Serial.print(duk_to_string(ctx, -1));
	return 0;
}

static duk_ret_t js_vibrate(duk_context *ctx) {
	watch.setWaveform(0, 14);
	watch.run();
	return 0;
}

static duk_ret_t js_delay(duk_context *ctx) {
	auto ms = duk_get_uint(ctx, 0);
	delay(ms);
	return 0;
}

static duk_ret_t js_getBrightness(duk_context *ctx) {
	auto brightness = watch.getBrightness();
	duk_push_uint(ctx, brightness);
	return 1;
}

static duk_ret_t js_setBrightness(duk_context *ctx) {
	auto brightness = duk_get_uint(ctx, 0);
	watch.setBrightness(brightness);
	return 0;
}

static duk_ret_t js_getAccelerometer(duk_context *ctx) {
	int16_t x, y, z;
	if (watch.getAccelerometer(x, y, z)) {
		duk_push_object(ctx);
		duk_push_int(ctx, x);
		duk_put_prop_string(ctx, -2, "x");
		duk_push_int(ctx, y);
		duk_put_prop_string(ctx, -2, "y");
		duk_push_int(ctx, z);
		duk_put_prop_string(ctx, -2, "z");
	} else
		duk_push_null(ctx);
	return 1;
}

void duktape_watch_install(duk_context *ctx) {
	duk_push_c_function(ctx, js_print, DUK_VARARGS);
	duk_put_global_string(ctx, "print");
	duk_push_c_function(ctx, js_vibrate, 0);
	duk_put_global_string(ctx, "vibrate");
	duk_push_c_function(ctx, js_delay, 1);
	duk_put_global_string(ctx, "delay");
	duk_push_c_function(ctx, js_getBrightness, 0);
	duk_put_global_string(ctx, "getBrightness");
	duk_push_c_function(ctx, js_setBrightness, 1);
	duk_put_global_string(ctx, "setBrightness");
	duk_push_c_function(ctx, js_getAccelerometer, 0);
	duk_put_global_string(ctx, "getAccelerometer");

	void duktape_watch_install_http(duk_context *ctx);
	duktape_watch_install_http(ctx);
	void duktape_watch_install_websocket(duk_context *ctx);
	duktape_watch_install_websocket(ctx);
	void duktape_watch_install_HttpServer(duk_context *ctx);
	duktape_watch_install_HttpServer(ctx);
	void duktape_watch_install_wifi(duk_context *ctx);
	duktape_watch_install_wifi(ctx);
	void duktape_watch_install_fs(duk_context *ctx);
	duktape_watch_install_fs(ctx);
	void duktape_watch_install_motor(duk_context *ctx);
	duktape_watch_install_motor(ctx);
}
