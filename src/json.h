#pragma once

#include <WString.h>
#include <vector>
#include <map>
#include <initializer_list>
#include <utility>

struct Json {
	enum struct Type {
		null,
		boolean,
		number,
		string,
		array,
		object
	};
	Type type;
	union {
		bool boolean;
		double number;
		String *string;
		std::vector<Json> *array;
		std::map<String, Json> *object;
	} value;
	static Json null;
	Json();
	Json(bool);
	Json(double);
	Json(const String &);
	Json(std::initializer_list<Json>);
	Json(std::initializer_list<std::pair<const String, Json>>);
	Json(const Json &);
	Json &operator=(const Json &);
	~Json();
	explicit operator bool() const;
	explicit operator double() const;
	explicit operator String() const;
	Json &operator[](int);
	std::size_t length() const;
	void push(const Json &);
	Json &operator[](const String &);
};
