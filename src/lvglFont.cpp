#include <lvgl.h>
#include <SPIFFS.h>

static lv_fs_res_t lvgl_spiffs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br) {
	auto file = (File *)file_p;
	*br = file->readBytes((char *)buf, btr);
	return LV_FS_RES_OK;
}

static void *lvgl_spiffs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
	auto file = new File(SPIFFS.open(path, mode == LV_FS_MODE_WR ? "w" : "r"));
	if (!file)
		return nullptr;
	return file;
}

static lv_fs_res_t lvgl_spiffs_close(lv_fs_drv_t * drv, void * file_p) {
	auto file = (File *)file_p;
	file->close();
	delete file;
	return LV_FS_RES_OK;
}

static lv_fs_res_t lvgl_spiffs_seek(_lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence) {
	auto file = (File *)file_p;
	file->seek(pos, (SeekMode)whence);
	return LV_FS_RES_OK;
}

void registerLvglFontDriver() {
	static lv_fs_drv_t lvgl_spiffs_drv;
	lv_fs_drv_init(&lvgl_spiffs_drv);

	lvgl_spiffs_drv.letter = 'S';
	lvgl_spiffs_drv.open_cb = lvgl_spiffs_open;
	lvgl_spiffs_drv.close_cb = lvgl_spiffs_close;
	lvgl_spiffs_drv.read_cb = lvgl_spiffs_read;
	lvgl_spiffs_drv.seek_cb = lvgl_spiffs_seek;

	lv_fs_drv_register(&lvgl_spiffs_drv);
}
