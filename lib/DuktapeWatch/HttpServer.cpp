#include <Arduino.h>
#include <duktape.h>
#include <esp_http_server.h>

static duk_ret_t js_HttpServer(duk_context *ctx) {
	duk_require_constructor_call(ctx);
	auto port = duk_get_uint(ctx, 0);
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.server_port = port;

	httpd_handle_t handle = NULL;
	auto err = httpd_start(&handle, &config);
	if (err)
		duk_generic_error(ctx, "http server start error: %d.", err);

	duk_push_this(ctx);
	duk_push_pointer(ctx, handle);
	duk_put_prop_string(ctx, -2, "handle");
	return 0;
}

static duk_ret_t js_HttpServer_stop(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (httpd_handle_t)duk_get_pointer(ctx, -1);
	httpd_stop(handle);
	return 0;
}

void push_HttpServer_prototype(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_HttpServer_stop, 0);
	duk_put_prop_string(ctx, -2, "stop");
}

void duktape_watch_install_HttpServer(duk_context *ctx) {
	duk_push_c_function(ctx, js_HttpServer, 1);
	push_HttpServer_prototype(ctx);
	duk_put_prop_string(ctx, -2, "prototype");
	duk_put_global_string(ctx, "HttpServer");
}
