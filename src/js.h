#include <duktape.h>

extern duk_context *jsContext;
void setupJs();
const char *evalJs(const char *code);
