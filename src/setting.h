#include <duktape.h>
#include "json.h"

class Setting
{
	const char *file;
	Json pop(duk_context *ctx);
	void push(duk_context *ctx, Json value);
public:
	Setting(const char *file);
	Json get(const char *path);
	void set(const char *path, Json value);
};

extern Setting setting;
extern Setting wifiNetworkSetting;
