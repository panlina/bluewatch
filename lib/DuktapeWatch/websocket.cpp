#include <Arduino.h>
#include <duktape.h>
#include <esp_websocket_client.h>

static duk_ret_t js_Client(duk_context *ctx) {
	duk_require_constructor_call(ctx);
	auto host = duk_get_string(ctx, 0);
	auto port = duk_get_int(ctx, 1);
	auto path = duk_get_string(ctx, 2);
	esp_websocket_client_config_t config{
		.host = host,
		.port = port,
		.path = path
	};
	auto handle = esp_websocket_client_init(&config);
	duk_push_this(ctx);
	duk_push_pointer(ctx, handle);
	duk_put_prop_string(ctx, -2, "handle");
	return 0;
}

static duk_ret_t js_Client_start(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (esp_websocket_client_handle_t)duk_get_pointer(ctx, -1);
	auto err = esp_websocket_client_start(handle);
	duk_push_int(ctx, err);
	return 1;
}

static duk_ret_t js_Client_send(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (esp_websocket_client_handle_t)duk_get_pointer(ctx, -1);
	auto data = duk_get_string(ctx, 0);
	auto timeout = (TickType_t)duk_get_uint_default(ctx, 1, portMAX_DELAY);
	auto n = esp_websocket_client_send_text(handle, data, strlen(data), timeout);
	duk_push_int(ctx, n);
	return 1;
}

static duk_ret_t js_Client_close(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (esp_websocket_client_handle_t)duk_get_pointer(ctx, -1);
	auto timeout = (TickType_t)duk_get_uint(ctx, 0);
	auto err = esp_websocket_client_close(handle, timeout);
	duk_push_int(ctx, err);
	return 1;
}

static duk_ret_t js_Client_destroy(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (esp_websocket_client_handle_t)duk_get_pointer(ctx, -1);
	auto err = esp_websocket_client_destroy(handle);
	duk_push_int(ctx, err);
	return 1;
}

void push_Client_prototype(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_Client_start, 0);
	duk_put_prop_string(ctx, -2, "start");
	duk_push_c_function(ctx, js_Client_send, 2);
	duk_put_prop_string(ctx, -2, "send");
	duk_push_c_function(ctx, js_Client_close, 1);
	duk_put_prop_string(ctx, -2, "close");
	duk_push_c_function(ctx, js_Client_destroy, 0);
	duk_put_prop_string(ctx, -2, "destroy");
}

void duktape_watch_install_websocket(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_Client, 3);
	push_Client_prototype(ctx);
	duk_put_prop_string(ctx, -2, "prototype");
	duk_put_prop_string(ctx, -2, "Client");
	duk_put_global_string(ctx, "websocket");
}
