#include <string>
#include <cmath>

#include "View/MainView.h"
#include "main.h"
#include "Log.h"

namespace view {


MainView::MainView()
	: chart_(ChartView::Instance())
{
	CreateContent();
}

MainView::~MainView()
{

}

Evas_Object *MainView::GetEvasObject()
{
	return win_;
}

void MainView::WinDeleteRequestCb(void *data, Evas_Object *obj, void *event_info)
{
	MainView *view = static_cast<MainView *>(data);
	elm_win_lower(view->win_);
}

void MainView::CreateContent()
{
	DBG("Create main content");

	win_ = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(win_, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(win_)) {
		int rots[2] = { 90, 270 };
		elm_win_wm_rotation_available_rotations_set(win_,
				(const int *)(&rots), sizeof(rots)/sizeof(rots[0]));
	}

	evas_object_smart_callback_add(win_, "delete,request", WinDeleteRequestCb, this);
	eext_object_event_callback_add(win_, EEXT_CALLBACK_BACK, WinDeleteRequestCb, this);
	elm_win_indicator_mode_set(win_, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(win_, ELM_WIN_INDICATOR_OPAQUE);

	conformant_ = elm_conformant_add(win_);
	evas_object_size_hint_weight_set(conformant_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win_, conformant_);
	evas_object_show(conformant_);

	evas_object_show(win_);

	layout_ = elm_layout_add(conformant_);

	std::string resourcePath = std::string(app_get_resource_path());
	resourcePath.append("edje/main.edj");

	if (!elm_layout_file_set(layout_, resourcePath.c_str(), "Main")) {
		ERR("Could not load file: %s", resourcePath.c_str());
		evas_object_del(win_);
		return;
	}
	elm_object_content_set(conformant_, layout_);

	evas_object_size_hint_weight_set(layout_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout_, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_show(layout_);

	CreateMenu();
	CreateDataMenu();

	evas_object_event_callback_add(layout_, EVAS_CALLBACK_RESIZE, LayoutResizeCb, this);
}

Evas_Object *MainView::CreateCursors()
{
	Evas_Object *cursors = elm_layout_add(layout_);
	std::string resourcePath = std::string(app_get_resource_path());
	resourcePath.append("edje/cursors.edj");

	if (!elm_layout_file_set(cursors, resourcePath.c_str(), "Cursors")) {
		ERR("Could not load file: %s", resourcePath.c_str());
		evas_object_del(cursors);
		return NULL;
	}

	evas_object_size_hint_weight_set(cursors, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(cursors, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_show(cursors);

	return cursors;
}

void MainView::CreateMenu()
{
	Evas_Object *menu = elm_layout_add(layout_);
	std::string resourcePath = std::string(app_get_resource_path());
	resourcePath.append("edje/menu.edj");

	if (!elm_layout_file_set(menu, resourcePath.c_str(), "Menu")) {
		ERR("Could not load file: %s", resourcePath.c_str());
		evas_object_del(menu);
		return;
	}

	evas_object_size_hint_weight_set(menu, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(menu, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_show(menu);
	elm_object_part_content_set(layout_, "menu", menu);

	elm_theme_overlay_add(NULL, resourcePath.c_str());

	Evas_Object *button = CreateMenuButton("V+");
	elm_object_part_content_set(menu, "btn.voltage.inc", button);
	button = CreateMenuButton("V-");
	elm_object_part_content_set(menu, "btn.voltage.dec", button);
	button = CreateMenuButton("T+");
	elm_object_part_content_set(menu, "btn.time.inc", button);
	button = CreateMenuButton("T-");
	elm_object_part_content_set(menu, "btn.time.dec", button);

	button = CreateMenuButton("Auto");
	elm_object_part_content_set(menu, "btn.auto", button);

	button = CreateMenuButton("CH2");
	elm_object_part_content_set(menu, "btn.ch", button);

}

void MainView::CreateDataMenu()
{
	Evas_Object *menu = elm_layout_add(layout_);
	std::string resourcePath = std::string(app_get_resource_path());
	resourcePath.append("edje/data.edj");

	if (!elm_layout_file_set(menu, resourcePath.c_str(), "Data")) {
		ERR("Could not load file: %s", resourcePath.c_str());
		evas_object_del(menu);
		return;
	}

	evas_object_size_hint_weight_set(menu, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(menu, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_show(menu);
	elm_object_part_content_set(layout_, "data", menu);
}

void MainView::ButtonClickedCb(void *data, Evas_Object *obj, void *event_info)
{
	DBG("");
}

Evas_Object *MainView::CreateMenuButton(const char *name)
{
	Evas_Object *button = elm_button_add(layout_);

	evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_smart_callback_add(button, "clicked", ButtonClickedCb, this);

	elm_object_text_set(button, name);

	evas_object_show(button);

	if (!elm_object_style_set(button, "scope"))
		ERR("elm_object_style_set failed");

	return button;
}

void MainView::LayoutResizeCb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	MainView *view = static_cast<MainView *>(data);

	view->CreateBg();

	Evas_Object *cursors = view->CreateCursors();
	elm_object_part_content_set(view->layout_, "cursors", cursors);

	Evas_Object *glView = view->chart_.CreateContent(view->layout_);
	elm_object_part_content_set(view->layout_, "trace", glView);
}

void MainView::CreateBg()
{

	static cairo_t *cairo_grid_;
	static cairo_surface_t *surface_grid_;

	if (grid_) {
		evas_object_del(grid_);
		grid_ = nullptr;
	}
	if (cairo_grid_) {
		cairo_destroy(cairo_grid_);
		cairo_grid_ = nullptr;
	}
	if (surface_grid_) {
		cairo_surface_destroy(surface_grid_);
		surface_grid_ = nullptr;
	}

	grid_ = evas_object_image_filled_add(evas_object_evas_get(layout_));
	evas_object_image_alpha_set(grid_, EINA_TRUE);
	evas_object_show(grid_);

	Evas_Object *ly = elm_layout_edje_get(layout_);
	edje_object_part_geometry_get(ly, "grid.bg", &lyX_, &lyY_, &lyW_, &lyH_);

	evas_object_geometry_set(grid_, lyX_, lyY_, lyW_, lyH_);
	evas_object_image_size_set(grid_, lyW_, lyH_);

	surface_grid_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, lyW_, lyH_);
	cairo_grid_ = cairo_create(surface_grid_);

	CreateGrid(cairo_grid_, surface_grid_);
	CreateXYAxis(cairo_grid_, surface_grid_);

	unsigned char *imageData = cairo_image_surface_get_data(cairo_get_target(cairo_grid_));
	evas_object_image_data_set(grid_, imageData);
	evas_object_image_data_update_add(grid_, lyX_, lyY_, lyW_, lyH_);

	evas_object_size_hint_weight_set(grid_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(grid_, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_part_content_set(layout_, "grid", grid_);
}

void MainView::CreateGrid(cairo_t *cairo, cairo_surface_t *surface)
{
	const double dashes[] = {
		8.33
	};

	cairo_set_source_rgba(cairo, 1.0, 1.0, 1.0, 0.6);
	cairo_set_line_width(cairo, 1.0);
	cairo_set_dash(cairo, dashes, sizeof(dashes)/sizeof(dashes[0]), -4.16);
	for (int i = 1; i < 6; ++i) {
		cairo_move_to(cairo, lyX_, lyY_ + lyH_ / 6 * i);
		cairo_line_to(cairo, lyX_ + lyW_, lyY_ + lyH_ / 6 * i);
	}

	for (int i = 1; i < 10; ++i) {
		cairo_move_to(cairo, lyX_ + lyW_ / 10 * i, lyY_);
		cairo_line_to(cairo, lyX_ + lyW_ / 10 * i, lyY_ + lyH_);
	}

	cairo_stroke(cairo);
	cairo_surface_flush(surface);
}

void MainView::CreateXYAxis(cairo_t *cairo, cairo_surface_t *surface)
{
	cairo_set_line_width(cairo, 2.0);
	cairo_set_dash(cairo, nullptr, 0, 0);
	cairo_set_source_rgba(cairo, 0.8, 0.8, 0.8, 1.0);

	cairo_move_to(cairo, lyX_ + lyW_/2.0, lyY_);
	cairo_line_to(cairo, lyX_ + lyW_/2.0, lyY_ + lyH_);

	cairo_move_to(cairo, lyX_, lyY_ + lyH_/2.0);
	cairo_line_to(cairo, lyX_ + lyW_, lyY_ + lyH_/2.0);

	cairo_stroke(cairo);
	cairo_surface_flush(surface);
}

void MainView::SetBuffer(std::vector<float> &buffer)
{
	buffer_ = buffer;
}

void MainView::SetYOffset(int offset)
{
	YOffset_ = offset;
}

}
