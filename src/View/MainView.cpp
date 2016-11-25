#include <string>

#include "View/MainView.h"
#include "main.h"
#include "Log.h"

namespace view {

MainView::MainView() {
	CreateContent();
}

Evas_Object *MainView::GetEvasObject() {
	return win_;
}

void MainView::CreateContent() {

	DBG("Create main content");

	win_ = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(win_, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(win_)) {
		int rots[2] = { 90, 270 };
		elm_win_wm_rotation_available_rotations_set(win_, (const int *)(&rots), sizeof(rots)/sizeof(rots[0]));
	}

	evas_object_smart_callback_add(win_, "delete,request", nullptr, NULL);
	eext_object_event_callback_add(win_, EEXT_CALLBACK_BACK, nullptr, NULL);
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

	evas_object_event_callback_add(layout_, EVAS_CALLBACK_RESIZE, LayoutResizeCb, this);
}

void MainView::CreateMenu() {

	Evas_Object *menu = elm_layout_add(layout_);
	std::string resourcePath = std::string(app_get_resource_path());
	resourcePath.append("edje/main.edj");

	if (!elm_layout_file_set(menu, resourcePath.c_str(), "Menu")) {
		ERR("Could not load file: %s", resourcePath.c_str());
		evas_object_del(win_);
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

void MainView::ButtonClickedCb(void *data, Evas_Object *obj, void *event_info) {
	DBG("");


}

Evas_Object *MainView::CreateMenuButton(const char *name) {

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

void MainView::LayoutResizeCb(void *data, Evas *e, Evas_Object *obj, void *event_info) {

	MainView *view = static_cast<MainView *>(data);

	view->CreateGrid();
}


void MainView::CreateGrid() {

	static cairo_t *cairo_grid_;
	static cairo_surface_t *surface_grid_;

	if (grid1_) {
		evas_object_del(grid1_);
		grid1_ = nullptr;
	}
	if (grid2_) {
		evas_object_del(grid2_);
		grid2_ = nullptr;
	}
	if (cairo_grid_) {
		cairo_destroy(cairo_grid_);
		cairo_grid_ = nullptr;
	}
	if (surface_grid_) {
		cairo_surface_destroy(surface_grid_);
		surface_grid_ = nullptr;
	}

	static double dashes[] = {
		10.0,
		10.0
	};

	grid1_ = evas_object_image_filled_add(evas_object_evas_get(layout_));
	evas_object_image_alpha_set(grid1_, EINA_TRUE);
	evas_object_show(grid1_);

	grid2_ = evas_object_image_filled_add(evas_object_evas_get(layout_));
	evas_object_image_alpha_set(grid2_, EINA_TRUE);
	evas_object_show(grid2_);

	int lyW, lyH, lyX, lyY;
	evas_object_geometry_get(layout_, &lyX, &lyY, &lyW, &lyH);
	DBG("Layout size: <%d %d> %dx%d", lyX, lyY, lyW, lyH);

	lyW = 1005;
	lyH = 335;

	evas_object_geometry_set(grid1_, 0, 0, lyW, lyH);
	evas_object_image_size_set(grid1_, lyW, lyH);

	evas_object_geometry_set(grid2_, 0, 0, lyW, lyH);
	evas_object_image_size_set(grid2_, lyW, lyH);

	surface_grid_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, lyW, lyH);
	cairo_grid_ = cairo_create(surface_grid_);

	cairo_set_source_rgba(cairo_grid_, 1.0, 1.0, 1.0, 0.6);
	cairo_set_line_width(cairo_grid_, 1.0);

	cairo_set_dash(cairo_grid_, dashes, sizeof(dashes)/sizeof(dashes[0]), -20.0);

	for (int i = 1; i < 5; ++i) {
		cairo_move_to(cairo_grid_, lyX, lyY + lyH / 5 * i);
		cairo_line_to(cairo_grid_, lyX + lyW, lyY + lyH / 5 * i);
	}

	for (int i = 1; i < 15; ++i) {
		cairo_move_to(cairo_grid_, lyX + lyW / 15 * i, lyY);
		cairo_line_to(cairo_grid_, lyX + lyW / 15 * i, lyY + lyH);
	}

	cairo_stroke(cairo_grid_);
	cairo_surface_flush(surface_grid_);

	unsigned char *imageData = cairo_image_surface_get_data(cairo_get_target(cairo_grid_));
	evas_object_image_data_set(grid1_, imageData);
	evas_object_image_data_update_add(grid1_, 0, 0, lyW, lyH);

	evas_object_image_data_set(grid2_, imageData);
	evas_object_image_data_update_add(grid2_, 0, 0, lyW, lyH);

	evas_object_size_hint_weight_set(grid1_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(grid1_, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_size_hint_weight_set(grid2_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(grid2_, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_part_content_set(layout_, "upper.grid", grid1_);
	elm_object_part_content_set(layout_, "lower.grid", grid2_);
}

}
