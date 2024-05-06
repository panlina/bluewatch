#include <LilyGoLib.h>
#include <duktape.h>
#include "sleep.h"

static duk_ret_t js_disableSleep_getter(duk_context *ctx) {
	duk_push_boolean(ctx, disableSleep);
	return 1;
}

static duk_ret_t js_disableSleep_setter(duk_context *ctx) {
	setDisableSleep(duk_get_boolean(ctx, 0));
	return 0;
}

void duktape_bluewatch_install(duk_context *ctx) {
	duk_push_global_object(ctx);
	duk_push_string(ctx, "disableSleep");
	duk_push_c_function(ctx, js_disableSleep_getter, 0);
	duk_push_c_function(ctx, js_disableSleep_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER);
	duk_pop(ctx);
}
