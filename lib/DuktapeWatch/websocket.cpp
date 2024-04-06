#include <Arduino.h>
#include <duktape.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

static duk_ret_t js_Client(duk_context *ctx) {
	duk_require_constructor_call(ctx);
	auto client = new WebsocketsClient();
	duk_push_this(ctx);
	duk_push_pointer(ctx, client);
	duk_put_prop_string(ctx, -2, "pointer");
	return 0;
}

static duk_ret_t js_Client_connect(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "pointer");
	auto client = (WebsocketsClient *)duk_get_pointer(ctx, -1);
	auto host = duk_get_string(ctx, 0);
	auto port = duk_get_int(ctx, 1);
	auto path = duk_get_string(ctx, 2);
	auto succeed = client->connect(host, port, path);
	duk_push_boolean(ctx, succeed);
	return 1;
}

static duk_ret_t js_Client_send(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "pointer");
	auto client = (WebsocketsClient *)duk_get_pointer(ctx, -1);
	auto data = duk_get_string(ctx, 0);
	auto succeed = client->send(data);
	duk_push_boolean(ctx, succeed);
	return 1;
}

static duk_ret_t js_Client_close(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "pointer");
	auto client = (WebsocketsClient *)duk_get_pointer(ctx, -1);
	client->close();
	return 0;
}

void push_Client_prototype(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_Client_connect, 3);
	duk_put_prop_string(ctx, -2, "connect");
	duk_push_c_function(ctx, js_Client_send, 1);
	duk_put_prop_string(ctx, -2, "send");
	duk_push_c_function(ctx, js_Client_close, 0);
	duk_put_prop_string(ctx, -2, "close");
}

void duktape_watch_install_websocket(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_Client, 0);
	push_Client_prototype(ctx);
	duk_put_prop_string(ctx, -2, "prototype");
	duk_put_prop_string(ctx, -2, "Client");
	duk_put_global_string(ctx, "websocket");
}
