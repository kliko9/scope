#include <string>
#include <cmath>
#include <ctime>

#include "cairo-evas-gl.h"

#include "View/MainView.h"
#include "main.h"
#include "Log.h"

namespace view {

MainView::MainView()
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
		elm_win_wm_rotation_available_rotations_set(win_, (const int *)(&rots), sizeof(rots)/sizeof(rots[0]));
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

	evas_object_event_callback_add(layout_, EVAS_CALLBACK_RESIZE, LayoutResizeCb, this);
}

void MainView::CreateMenu()
{

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

	//evas_object_show(menu);
	//elm_object_part_content_set(layout_, "menu", menu);

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
	view->CreateTrace();
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
	//evas_object_geometry_get(layout_, &lyX_, &lyY_, &lyW_, &lyH_);
	//DBG("Layout size: <%d %d> %dx%d", lyX_, lyY_, lyW_, lyH_);

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

void MainView::CairoDrawCb(void *data, Evas_Object *obj)
{
	std::clock_t begin = std::clock();
	MainView *view = static_cast<MainView *>(data);

	if (view->buffer_.size() == 0) {
		DBG("Data buffer is empty");
		return;
	}

	cairo_set_source_rgba(view->cairo_trace_, 0.0, 0.0, 0.0, 1.0);
	cairo_paint(view->cairo_trace_);

	cairo_set_source_rgba(view->cairo_trace_, 1.0, 1.0, 0.2, 1.0);
	cairo_set_line_width(view->cairo_trace_, 3.0);

	cairo_set_operator(view->cairo_trace_, CAIRO_OPERATOR_OVER);
	cairo_move_to(view->cairo_trace_, view->lyX_, view->lyY_ + view->YOffset_ + view->lyH_/2.0 - (view->buffer_[0] * view->lyH_/2.0));

	for (int i = 1; i <= view->buffer_.size(); i++) {
		if (i % 2) //TODO remove
			continue;

		cairo_line_to(view->cairo_trace_, view->lyX_ + view->lyW_*(i/(double)view->buffer_.size()), view->lyY_ + view->YOffset_ + view->lyH_/2.0 - view->buffer_[i] * view->lyH_/2.0);
	}

	cairo_stroke(view->cairo_trace_);
	cairo_surface_flush(view->surface_trace_);

	DBG("1 frame took: %f s", (double)(std::clock() - begin)/CLOCKS_PER_SEC);
}

Eina_Bool MainView::AnimateCb(void *data)
{
	MainView *view = static_cast<MainView *>(data);

	evas_object_image_pixels_dirty_set(view->trace_, EINA_TRUE);

	return EINA_TRUE;
}

void MainView::SetBuffer(std::vector<double> &buffer)
{
	buffer_ = buffer;
}

void MainView::CreateTrace()
{
	std::clock_t init;
	std::clock_t drawing;
	std::clock_t begin = std::clock();

	if (!trace_) {
		trace_ = evas_object_image_filled_add(evas_object_evas_get(layout_));
		evas_object_image_alpha_set(trace_, EINA_TRUE);

		evas_object_show(trace_);
	}

	Evas_Object *ly = elm_layout_edje_get(layout_);
	edje_object_part_geometry_get(ly, "grid.bg", &lyX_, &lyY_, &lyW_, &lyH_);
	//DBG("Layout size: <%d %d> %dx%d", lyX_, lyY_, lyW_, lyH_);

	evas_object_geometry_set(trace_, lyX_, lyY_, lyW_, lyH_);
	evas_object_image_size_set(trace_, lyW_, lyH_);

	if (!evas_gl_)
		evas_gl_ = evas_gl_new(evas_object_evas_get(trace_));
	if (!evas_gl_config_)
		evas_gl_config_ = evas_gl_config_new();

	evas_gl_config_->color_format = EVAS_GL_RGBA_8888;

	if (!evas_gl_surface_)
		evas_gl_surface_ = evas_gl_surface_create(evas_gl_, evas_gl_config_, lyW_, lyH_);

	if (!evas_gl_context_)
		evas_gl_context_ = evas_gl_context_create(evas_gl_, NULL);

	evas_gl_native_surface_get(evas_gl_, evas_gl_surface_, &ns_);

	evas_object_image_native_surface_set(trace_, &ns_);
	evas_object_image_pixels_get_callback_set(trace_, CairoDrawCb, this);

	setenv("CAIRO_GL_COMPOSITOR", "msaa", 1);
	if (!cairo_device_)
		cairo_device_ = (cairo_device_t *)cairo_evas_gl_device_create(evas_gl_, evas_gl_context_);

	cairo_gl_device_set_thread_aware(cairo_device_, 0);
	if (!surface_trace_)
		surface_trace_ = (cairo_surface_t *)cairo_gl_surface_create_for_evas_gl(cairo_device_, evas_gl_surface_, evas_gl_config_, lyW_, lyH_);

	if (!cairo_trace_)
		cairo_trace_ = cairo_create(surface_trace_);

	init = std::clock();
	drawing = std::clock();

	evas_object_size_hint_weight_set(trace_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(trace_, EVAS_HINT_FILL, EVAS_HINT_FILL);

	DBG("BENCHMARK:");
	DBG("init: %f", (double)(init - begin)/CLOCKS_PER_SEC);
	DBG("drawing: %f", (double)(drawing - init)/CLOCKS_PER_SEC);
	DBG("updating: %f", (double)(std::clock() - drawing)/CLOCKS_PER_SEC);

	ecore_animator_frametime_set(0.03);
	if (!animator_)
		animator_ = ecore_animator_add(AnimateCb, this);
}

void MainView::SetYOffset(int offset)
{
	YOffset_ = offset;
}

}
