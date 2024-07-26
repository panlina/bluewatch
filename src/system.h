#include <lvgl.h>

class App {
protected:
	lv_obj_t *tile;
public:
	virtual void create(lv_obj_t *tile);
	virtual void destroy();
};

void setupSystem();
