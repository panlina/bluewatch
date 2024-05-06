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

static duk_ret_t js_exists(duk_context *ctx) {
	auto path = duk_get_string(ctx, 0);
	duk_push_boolean(ctx, SPIFFS.exists(path));
	return 1;
}

static duk_ret_t js_remove(duk_context *ctx) {
	auto path = duk_get_string(ctx, 0);
	duk_push_boolean(ctx, SPIFFS.remove(path));
	return 1;
}

static duk_ret_t js_rename(duk_context *ctx) {
	auto from = duk_get_string(ctx, 0);
	auto to = duk_get_string(ctx, 1);
	duk_push_boolean(ctx, SPIFFS.rename(from, to));
	return 1;
}

static duk_ret_t js_listDir(duk_context *ctx) {
	auto path = duk_get_string(ctx, 0);

	auto root = SPIFFS.open(path);
	auto file = root.getNextFileName();
	auto i = 0;
	duk_push_array(ctx);
	while (!file.isEmpty()) {
		duk_push_string(ctx, file.c_str());
		duk_put_prop_index(ctx, -2, i++);
		file = root.getNextFileName();
	}

	return 1;
}

void duktape_watch_install_fs(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_readFile, 1);
	duk_put_prop_string(ctx, -2, "readFile");
	duk_push_c_function(ctx, js_writeFile, 2);
	duk_put_prop_string(ctx, -2, "writeFile");
	duk_push_c_function(ctx, js_exists, 1);
	duk_put_prop_string(ctx, -2, "exists");
	duk_push_c_function(ctx, js_remove, 1);
	duk_put_prop_string(ctx, -2, "remove");
	duk_push_c_function(ctx, js_rename, 2);
	duk_put_prop_string(ctx, -2, "rename");
	duk_push_c_function(ctx, js_listDir, 1);
	duk_put_prop_string(ctx, -2, "listDir");
	duk_put_global_string(ctx, "fs");
}
