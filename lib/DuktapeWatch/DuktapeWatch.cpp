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

void duktape_watch_install(duk_context *ctx) {
	duk_push_c_function(ctx, js_print, DUK_VARARGS);
	duk_put_global_string(ctx, "print");
	duk_push_c_function(ctx, js_vibrate, 0);
	duk_put_global_string(ctx, "vibrate");
	duk_push_c_function(ctx, js_delay, 1);
	duk_put_global_string(ctx, "delay");

	void duktape_watch_install_http(duk_context *ctx);
	duktape_watch_install_http(ctx);
	void duktape_watch_install_wifi(duk_context *ctx);
	duktape_watch_install_wifi(ctx);
}
