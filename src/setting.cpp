#include <SPIFFS.h>
#include <duktape.h>
#include "setting.h"
#include "json.h"

Setting::Setting(const char *file) : file(file) {}

Json Setting::get(const char *path) {
	auto ctx = duk_create_heap_default();
	auto file = SPIFFS.open(this->file);
	auto content = file.readString();
	file.close();
	duk_eval_string(ctx, ("(" + content + path + ")").c_str());
	Json value = pop(ctx);
	duk_destroy_heap(ctx);
	return value;
}

void Setting::set(const char *path, Json value) {
	auto ctx = duk_create_heap_default();
	auto file = SPIFFS.open(this->file);
	auto content = file.readString();
	file.close();
	duk_push_string(ctx, content.c_str());
	duk_json_decode(ctx, -1);
	duk_put_global_string(ctx, "a");
	push(ctx, value);
	duk_put_global_string(ctx, "b");
	duk_eval_string(ctx, ("a" + String(path) + "=b,a").c_str());
	duk_json_encode(ctx, -1);
	content = (String)pop(ctx).value.string;
	file = SPIFFS.open(this->file, "w");
	file.write((const uint8_t *)content.c_str(), content.length());
	file.close();
	duk_destroy_heap(ctx);
}

Json Setting::pop(duk_context *ctx) {
	Json value;
	value.type = duk_get_type(ctx, -1);
	switch (value.type)
	{
	case DUK_TYPE_BOOLEAN:
		value.value.boolean = duk_get_boolean(ctx, -1);
		break;
	case DUK_TYPE_NUMBER:
		value.value.number = duk_get_number(ctx, -1);
		break;
	case DUK_TYPE_STRING:
		value.value.string = duk_get_string(ctx, -1);
		break;
	}
	duk_pop(ctx);
	return value;
}

void Setting::push(duk_context *ctx, Json value) {
	switch (value.type)
	{
	case DUK_TYPE_UNDEFINED:
		duk_push_undefined(ctx);
		break;
	case DUK_TYPE_NULL:
		duk_push_null(ctx);
		break;
	case DUK_TYPE_BOOLEAN:
		duk_push_boolean(ctx, value.value.boolean);
		break;
	case DUK_TYPE_NUMBER:
		duk_push_number(ctx, value.value.number);
		break;
	case DUK_TYPE_STRING:
		duk_push_string(ctx, value.value.string);
		break;
	default:
		break;
	}
}

Setting setting("/setting.json");
