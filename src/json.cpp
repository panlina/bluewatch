#include <duktape.h>
#include "json.h"

Json::Json() {}
Json::Json(bool value) : type(DUK_TYPE_BOOLEAN), value({boolean : value}) {}
Json::Json(double value) : type(DUK_TYPE_NUMBER), value({number : value}) {}
Json::Json(const char *value) : type(DUK_TYPE_STRING), value({string : value}) {}
Json::operator bool() const { return value.boolean; }
Json::operator double() const { return value.number; }
Json::operator const char *() const { return value.string; }
