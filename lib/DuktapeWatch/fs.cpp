#include <duktape.h>
#include "SPIFFS.h"

static duk_ret_t js_readFile(duk_context *ctx) {
	auto path = duk_get_string(ctx, 0);

	auto file = SPIFFS.open(path);
	auto s = file.readString();
	file.close();
	duk_push_string(ctx, s.c_str());

	return 1;
}

static duk_ret_t js_writeFile(duk_context *ctx) {
	auto path = duk_get_string(ctx, 0);
	auto s = duk_get_string(ctx, 1);

	auto file = SPIFFS.open(path, "w");
	auto n = file.write((uint8_t *)s, strlen(s));
	file.close();
	duk_push_uint(ctx, n);

	return 1;
}

void duktape_watch_install_fs(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_readFile, 1);
	duk_put_prop_string(ctx, -2, "readFile");
	duk_push_c_function(ctx, js_writeFile, 2);
	duk_put_prop_string(ctx, -2, "writeFile");
	duk_put_global_string(ctx, "fs");
}
