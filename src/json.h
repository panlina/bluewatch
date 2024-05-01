#pragma once

struct Json {
	int type;
	union {
		bool boolean;
		double number;
		const char *string;
	} value;
	Json();
	Json(bool);
	Json(double);
	Json(const char *);
	explicit operator bool() const;
	explicit operator double() const;
	explicit operator const char *() const;
};
