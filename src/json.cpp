#include <WString.h>
#include <vector>
#include <map>
#include <initializer_list>
#include <utility>
#include "json.h"

using std::vector;
using std::map;
using std::initializer_list;
using std::pair;
using std::size_t;

Json Json::null = Json();
Json::Json() : type(Json::Type::null) {}
Json::Json(bool value) : type(Json::Type::boolean), value({boolean : value}) {}
Json::Json(double value) : type(Json::Type::number), value({number : value}) {}
Json::Json(const String &value) : type(Json::Type::string), value({string : new String(value)}) {}
Json::Json(initializer_list<Json> value) : type(Json::Type::array), value({array : new vector<Json>(value)}) {}
Json::Json(initializer_list<pair<const String, Json>> value) : type(Json::Type::object), value({object : new map<String, Json>(value)}) {}
Json::Json(const Json &that) {
	type = that.type;
	switch (type) {
	case Type::boolean: value.boolean = that.value.boolean; break;
	case Type::number: value.number = that.value.number; break;
	case Type::string: value.string = new String(*that.value.string); break;
	case Type::array: value.array = new vector<Json>(*that.value.array); break;
	case Type::object: value.object = new map<String, Json>(*that.value.object); break;
	}
}
Json &Json::operator=(const Json &that) {
	switch (type) {
	case Type::string: delete value.string; break;
	case Type::array: delete value.array; break;
	case Type::object: delete value.object; break;
	}
	type = that.type;
	switch (type) {
	case Type::boolean: value.boolean = that.value.boolean; break;
	case Type::number: value.number = that.value.number; break;
	case Type::string: value.string = new String(*that.value.string); break;
	case Type::array: value.array = new vector<Json>(*that.value.array); break;
	case Type::object: value.object = new map<String, Json>(*that.value.object); break;
	}
	return *this;
}
Json::~Json() {
	switch (type) {
	case Type::string: delete value.string; break;
	case Type::array: delete value.array; break;
	case Type::object: delete value.object; break;
	}
}
Json::operator bool() const { return value.boolean; }
Json::operator double() const { return value.number; }
Json::operator String() const { return *value.string; }
Json &Json::operator[](int index) { return (*value.array)[index]; }
size_t Json::length() const { return value.array->size(); }
void Json::push(const Json &element) { value.array->push_back(element); }
Json &Json::operator[](const String &key) { return (*value.object)[key]; }
