#pragma once

#include <vector>
#include <map>
#include <initializer_list>
#include <utility>

struct Json {
	enum Type {
		undefined,
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
		const char *string;
		std::vector<Json> *array;
		std::map<const char *, Json> *object;
	} value;
	Json();
	Json(bool);
	Json(double);
	Json(const char *);
	Json(std::initializer_list<Json>);
	Json(std::initializer_list<std::pair<const char *const, Json>>);
	Json(const Json &);
	Json &operator=(const Json &);
	~Json();
	explicit operator bool() const;
	explicit operator double() const;
	explicit operator const char *() const;
	Json &operator[](int);
	std::size_t length() const;
	void push(const Json &);
	Json &operator[](const char *);
};
