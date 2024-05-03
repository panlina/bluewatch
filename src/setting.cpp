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
	content = (String)pop(ctx);
	file = SPIFFS.open(this->file, "w");
	file.write((const uint8_t *)content.c_str(), content.length());
	file.close();
	duk_destroy_heap(ctx);
}

Json Setting::pop(duk_context *ctx) {
	Json value;
	auto type = duk_get_type(ctx, -1);
	switch (type)
	{
	case DUK_TYPE_BOOLEAN:
		value.type = Json::Type::boolean;
		value.value.boolean = duk_get_boolean(ctx, -1);
		break;
	case DUK_TYPE_NUMBER:
		value.type = Json::Type::number;
		value.value.number = duk_get_number(ctx, -1);
		break;
	case DUK_TYPE_STRING:
		value.type = Json::Type::string;
		value.value.string = new String(duk_get_string(ctx, -1));
		break;
	case DUK_TYPE_OBJECT:
		if (duk_is_array(ctx, -1)) {
			value = Json((std::initializer_list<Json>){});
			auto length = duk_get_length(ctx, -1);
			for (auto i = 0; i < length; i++) {
				duk_get_prop_index(ctx, -1, i);
				value.push(pop(ctx));
			}
		} else {
			value = Json((std::initializer_list<std::pair<const String, Json>>){});
			duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);
			while (duk_next(ctx, -1, 1)) {
				auto _value = pop(ctx);
				auto key = duk_get_string(ctx, -1);
				value[key] = _value;
				duk_pop(ctx);
			}
			duk_pop(ctx);
		}
		break;
	}
	duk_pop(ctx);
	return value;
}

void Setting::push(duk_context *ctx, Json value) {
	switch (value.type)
	{
	case Json::Type::null:
		duk_push_null(ctx);
		break;
	case Json::Type::boolean:
		duk_push_boolean(ctx, value.value.boolean);
		break;
	case Json::Type::number:
		duk_push_number(ctx, value.value.number);
		break;
	case Json::Type::string:
		duk_push_string(ctx, value.value.string->c_str());
		break;
	case Json::Type::array: {
		duk_push_array(ctx);
		auto length = value.length();
		for (auto i = 0; i < length; i++) {
			push(ctx, value[i]);
			duk_put_prop_index(ctx, -2, i);
		}
		break;
	}
	case Json::Type::object:
		duk_push_object(ctx);
		for (auto i = value.value.object->begin(); i != value.value.object->end(); i++) {
			push(ctx, i->second);
			duk_put_prop_string(ctx, -2, i->first.c_str());
		}
		break;
	default:
		break;
	}
}

Setting setting("/setting.json");
Setting wifiNetworkSetting("/wifi-network.json");
