#include "json.h"

Json::Json() {}
Json::Json(bool value) : type(Json::Type::boolean), value({boolean : value}) {}
Json::Json(double value) : type(Json::Type::number), value({number : value}) {}
Json::Json(const char *value) : type(Json::Type::string), value({string : value}) {}
Json::operator bool() const { return value.boolean; }
Json::operator double() const { return value.number; }
Json::operator const char *() const { return value.string; }
