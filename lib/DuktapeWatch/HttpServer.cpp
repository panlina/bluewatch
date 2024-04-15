#include <Arduino.h>
#include <duktape.h>
#include <esp_http_server.h>
#include <functional>

using std::function;

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

	duk_push_array(ctx);
	duk_put_prop_string(ctx, -2, "handlers");
	return 0;
}

/*
	Duktape `HttpServer`s use `user_ctx` as a general means to maintain handler contexts,
	eliminating any need to pass context data explicitly.

	Handlers are put to the stash, using the addresses of wrapper `std::function` instances
	as keys, and are deleted when the server is stopped.
*/
static duk_ret_t js_HttpServer_stop(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (httpd_handle_t)duk_get_pointer(ctx, -1);

	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handlers");

	auto n = duk_get_length(ctx, -1);
	duk_push_global_stash(ctx);
	// [..., handlers, stash]
	for (duk_size_t i = 0; i < n; i++) {
		duk_get_prop_index(ctx, -2, i);
		auto user_ctx = (function<esp_err_t(httpd_req_t *)> *)duk_get_pointer(ctx, -1);
		delete user_ctx;
		duk_pop(ctx);
		char stashKey[32];
		sprintf(stashKey, "httpd_handler_%x", user_ctx);
		duk_del_prop_string(ctx, -1, stashKey);
	}

	httpd_stop(handle);
	return 0;
}

static duk_ret_t js_HttpServer_registerUriHandler(duk_context *ctx) {
	auto uri = duk_get_string(ctx, 0);
	auto method = (httpd_method_t)duk_get_uint(ctx, 1);
	duk_push_global_stash(ctx);
	duk_dup(ctx, 2);
	auto user_ctx = new function<esp_err_t(httpd_req_t *)>([=](httpd_req_t *req) {
		auto user_ctx = req->user_ctx;
		duk_push_global_stash(ctx);
		char stashKey[32];
		sprintf(stashKey, "httpd_handler_%x", user_ctx);
		duk_get_prop_string(ctx, -1, stashKey);
		duk_push_pointer(ctx, req);
		duk_get_global_string(ctx, "HttpServer.Response");
		duk_push_pointer(ctx, req);
		duk_new(ctx, 1);
		duk_call(ctx, 2);
		duk_pop_2(ctx);
		return ESP_OK;
	});
	char stashKey[32];
	sprintf(stashKey, "httpd_handler_%x", user_ctx);
	duk_put_prop_string(ctx, -2, stashKey);
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handlers");
	duk_push_pointer(ctx, user_ctx);
	auto length = duk_get_length(ctx, -2);
	duk_put_prop_index(ctx, -2, length);
	httpd_uri_t uriHandler = {
		.uri = uri,
		.method = method,
		.handler = [](httpd_req_t *req) {
			auto user_ctx = (function<esp_err_t(httpd_req_t *)> *)req->user_ctx;
			return (*user_ctx)(req);
		},
		.user_ctx = user_ctx
	};
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (httpd_handle_t)duk_get_pointer(ctx, -1);
	auto err = httpd_register_uri_handler(handle, &uriHandler);
	if (err)
		duk_generic_error(ctx, "http server register uri handler error: %d.", err);
	return 0;
}

static duk_ret_t js_HttpServer_registerUriHandlerWebsocket(duk_context *ctx) {
	auto uri = duk_get_string(ctx, 0);
	auto method = (httpd_method_t)duk_get_uint(ctx, 1);
	duk_push_global_stash(ctx);
	duk_dup(ctx, 2);
	auto user_ctx = new function<esp_err_t(httpd_req_t *)>([=](httpd_req_t *req) {
		if (req->method == method) {
			auto user_ctx = req->user_ctx;
			duk_push_global_stash(ctx);
			char stashKey[32];
			sprintf(stashKey, "httpd_handler_%x", user_ctx);
			duk_get_prop_string(ctx, -1, stashKey);
			duk_get_global_string(ctx, "HttpServer.Socket");
			duk_push_pointer(ctx, req->handle);
			auto fd = httpd_req_to_sockfd(req);
			duk_push_int(ctx, fd);
			duk_new(ctx, 2);
			duk_call(ctx, 1);
			duk_pop_2(ctx);
			return ESP_OK;
		}
	});
	char stashKey[32];
	sprintf(stashKey, "httpd_handler_%x", user_ctx);
	duk_put_prop_string(ctx, -2, stashKey);
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handlers");
	duk_push_pointer(ctx, user_ctx);
	auto length = duk_get_length(ctx, -2);
	duk_put_prop_index(ctx, -2, length);
	httpd_uri_t uriHandler = {
		.uri = uri,
		.method = (httpd_method_t)method,
		.handler = [](httpd_req_t *req) {
			auto user_ctx = (function<esp_err_t(httpd_req_t *)> *)req->user_ctx;
			return (*user_ctx)(req);
		},
		.user_ctx = user_ctx,
		.is_websocket = true
	};
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (httpd_handle_t)duk_get_pointer(ctx, -1);
	auto err = httpd_register_uri_handler(handle, &uriHandler);
	if (err)
		duk_generic_error(ctx, "http server register uri handler error: %d.", err);
	return 0;
}

void push_HttpServer_prototype(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_HttpServer_stop, 0);
	duk_put_prop_string(ctx, -2, "stop");
	duk_push_c_function(ctx, js_HttpServer_registerUriHandler, 3);
	duk_put_prop_string(ctx, -2, "registerUriHandler");
	duk_push_c_function(ctx, js_HttpServer_registerUriHandlerWebsocket, 3);
	duk_put_prop_string(ctx, -2, "registerUriHandlerWebsocket");
}

static duk_ret_t js_Response(duk_context *ctx) {
	duk_require_constructor_call(ctx);
	auto req = duk_get_pointer(ctx, 0);

	duk_push_this(ctx);
	duk_push_pointer(ctx, req);
	duk_put_prop_string(ctx, -2, "req");

	return 0;
}

static duk_ret_t js_Response_send(duk_context *ctx) {
	auto text = duk_get_string(ctx, 0);

	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "req");
	auto req = (httpd_req_t *)duk_get_pointer(ctx, -1);

	auto err = httpd_resp_send(req, text, HTTPD_RESP_USE_STRLEN);
	if (err)
		duk_generic_error(ctx, "http response send error: %d.", err);
	return 0;
}

void push_Response_prototype(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_Response_send, 1);
	duk_put_prop_string(ctx, -2, "send");
}

static duk_ret_t js_Socket(duk_context *ctx) {
	duk_require_constructor_call(ctx);
	auto handle = duk_get_pointer(ctx, 0);
	auto fd = duk_get_int(ctx, 1);

	duk_push_this(ctx);
	duk_push_pointer(ctx, handle);
	duk_put_prop_string(ctx, -2, "handle");
	duk_push_int(ctx, fd);
	duk_put_prop_string(ctx, -2, "fd");

	return 0;
}

static duk_ret_t js_Socket_send(duk_context *ctx) {
	auto text = duk_get_string(ctx, 0);

	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "handle");
	auto handle = (httpd_handle_t)duk_get_pointer(ctx, -1);

	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "fd");
	auto fd = duk_get_int(ctx, -1);

	httpd_ws_frame_t ws_pkt;
	ws_pkt.type = HTTPD_WS_TYPE_TEXT;
	ws_pkt.payload = (uint8_t *)text;
	ws_pkt.len = strlen(text);
	auto err = httpd_ws_send_frame_async(handle, fd, &ws_pkt);
	if (err)
		duk_generic_error(ctx, "socket send error: %d.", err);
	return 0;
}

void push_Socket_prototype(duk_context *ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_Socket_send, 1);
	duk_put_prop_string(ctx, -2, "send");
}

void duktape_watch_install_HttpServer(duk_context *ctx) {
	duk_push_c_function(ctx, js_HttpServer, 1);
	push_HttpServer_prototype(ctx);
	duk_put_prop_string(ctx, -2, "prototype");
	duk_put_global_string(ctx, "HttpServer");
	duk_push_c_function(ctx, js_Response, 1);
	push_Response_prototype(ctx);
	duk_put_prop_string(ctx, -2, "prototype");
	duk_put_global_string(ctx, "HttpServer.Response");
	duk_push_c_function(ctx, js_Socket, 2);
	push_Socket_prototype(ctx);
	duk_put_prop_string(ctx, -2, "prototype");
	duk_put_global_string(ctx, "HttpServer.Socket");
	duk_push_uint(ctx, HTTP_DELETE);
	duk_put_global_string(ctx, "HTTP_DELETE");
	duk_push_uint(ctx, HTTP_GET);
	duk_put_global_string(ctx, "HTTP_GET");
	duk_push_uint(ctx, HTTP_HEAD);
	duk_put_global_string(ctx, "HTTP_HEAD");
	duk_push_uint(ctx, HTTP_POST);
	duk_put_global_string(ctx, "HTTP_POST");
	duk_push_uint(ctx, HTTP_PUT);
	duk_put_global_string(ctx, "HTTP_PUT");
}
