#include <Arduino.h>
#include <duktape.h>
#include <HTTPClient.h>

struct Response {
	int code;
	String body;
};

static Response http(const char *method, String url, String *payload)
{
	HTTPClient http;
	http.begin(url);
	int code = payload ? http.sendRequest(method, *payload) : http.sendRequest(method);

	String body;
	if (code > 0)
		body = http.getString();

	http.end();

	return Response {
		.code = code,
		.body = body
	};
}

static duk_ret_t js_http(duk_context *ctx) {
	auto method = duk_get_string(ctx, 0);
	auto url = duk_get_string(ctx, 1);
	auto payload = duk_get_string(ctx, 2);
	String payloadString;
	if (payload)
		payloadString = String(payload);
	auto response = http(method, url, payload ? &payloadString : nullptr);

	if (response.code < 0)
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "http exception. error code: %d.", response.code);
	else if (response.code >= 400)
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "http exception. error code: %d.%s", response.code, response.body ? ('\n' + response.body).c_str() : "");
	else
		duk_push_object(ctx);

	duk_push_int(ctx, response.code);
	duk_put_prop_string(ctx, -2, "code");
	duk_push_string(ctx, response.body.c_str());
	duk_put_prop_string(ctx, -2, "body");

	if (response.code < 0 || response.code >= 400)
		duk_throw(ctx);

	return 1;
}

void duktape_watch_install_http(duk_context *ctx) {
	duk_push_c_function(ctx, js_http, 3);
	duk_put_global_string(ctx, "http");
}
