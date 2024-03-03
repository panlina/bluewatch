#include <LilyGoLib.h>
#include <duktape.h>

duk_context *jsContext;

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

void setupJs()
{
	jsContext = duk_create_heap_default();

	duk_push_c_function(jsContext, js_print, DUK_VARARGS);
	duk_put_global_string(jsContext, "print");
	duk_push_c_function(jsContext, js_vibrate, 0);
	duk_put_global_string(jsContext, "vibrate");
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
