#include <LilyGoLib.h>
#include <duktape.h>

static duk_ret_t js_setWaveform(duk_context *ctx) {
	auto slot = duk_get_uint(ctx, 0);
	auto w = duk_get_uint(ctx, 1);
	watch.setWaveform(slot, w);
	return 0;
}

static duk_ret_t js_run(duk_context *ctx) {
	watch.run();
	return 0;
}

static duk_ret_t js_stop(duk_context *ctx) {
	watch.stop();
	return 0;
}

void duktape_watch_install_motor(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_setWaveform, 2);
	duk_put_prop_string(ctx, -2, "setWaveform");
	duk_push_c_function(ctx, js_run, 0);
	duk_put_prop_string(ctx, -2, "run");
	duk_push_c_function(ctx, js_stop, 0);
	duk_put_prop_string(ctx, -2, "stop");
	duk_put_global_string(ctx, "motor");
}
