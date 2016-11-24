#include "main.h"
#include "Log.h"
#include "Controller/MainController.h"

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
}

static void
create_base_gui(void)
{
	controller::MainController::GetInstance().Init();
}

static bool
app_create(void *data)
{
	create_base_gui();

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
}

static void
app_pause(void *data)
{
}

static void
app_resume(void *data)
{
}

static void
app_terminate(void *data)
{
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	int ret;
	char *language;

	ret = app_event_get_language(event_info, &language);
	if (ret != APP_ERROR_NONE) {
		DBG("app_event_get_language() failed. Err = %d.", ret);
		return;
	}

	if (language != NULL) {
		elm_language_set(language);
		free(language);
	}
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
}

int
main(int argc, char *argv[])
{
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, nullptr);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, nullptr);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, nullptr);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, nullptr);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, nullptr);

	ret = ui_app_main(argc, argv, &event_callback, nullptr);
	if (ret != APP_ERROR_NONE) {
		DBG("app_main() is failed. err = %d", ret);
	}

	return ret;
}
