#include <duktape.h>

struct Value;

class Setting
{
	const char *file;
	Value pop(duk_context *ctx);
	void push(duk_context *ctx, Value value);
public:
	Setting(const char *file);
	Value get(const char *path);
	void set(const char *path, Value value);
};

extern Setting setting;

struct Value {
	int type;
	union {
		bool boolean;
		double number;
		const char *string;
	} value;
	Value();
	Value(bool);
	Value(double);
	Value(const char *);
	explicit operator bool() const;
	explicit operator double() const;
	explicit operator const char *() const;
};
