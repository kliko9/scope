#include <string>
#include <cmath>
#include <ctime>
#include <cstdlib>

#include "View/MainView.h"
#include "main.h"
#include "Log.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

namespace view {

const char *vertexShaderSrc =
	"attribute vec4 a_position;\n"
	"void main() {\n"
	"	gl_Position = vec4(vec2(a_position), 0.0, 1.0);\n"
	"}\n"
	;

const char *fragmentShaderSrc =
	"#ifdef GL_ES\n"
	"precision mediump float;\n"
	"#endif\n"
	"void main() {\n"
	"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n"
	;

struct Point {
	GLfloat x;
	GLfloat y;
};

Point triangleVectices[2000];

ELEMENTARY_GLVIEW_GLOBAL_DEFINE();

MainView::MainView()
{
	CreateContent();
}

MainView::~MainView()
{

}

MainView &MainView::Instance()
{
	static MainView instance;
	return instance;
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

void MainView::SetViewport(int w, int h)
{
	viewport_h = h;
	viewport_w = w;
}

void MainView::Viewport(int *w, int *h)
{
	*h = viewport_h;
	*w = viewport_w;
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

Eina_Bool MainView::AnimateCb(void *data)
{
	MainView *view = static_cast<MainView *>(data);

	elm_glview_changed_set(view->gl_view_);

	return EINA_TRUE;
}

void MainView::SetBuffer(std::vector<float> &buffer)
{
	buffer_ = buffer;
}

bool MainView::GLInitShaders()
{
	char log[256] = {0, };

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);
	glGetShaderInfoLog(vertexShader, sizeof(log), NULL, log);
	DBG("Shader compilation log: %s", log);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderInfoLog(fragmentShader, sizeof(log), NULL, log);
	DBG("Shader compilation log: %s", log);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	attrPosition = glGetAttribLocation(program, "a_position");

	glUseProgram(program);

	return true;
}

unsigned short MainView::AttrPosition()
{
	return attrPosition;
}

void MainView::GLInitCb(Evas_Object *obj)
{
	DBG("Init gl");

	MainView::Instance().GLInitShaders();
}

void MainView::GLRenderCb(Evas_Object *obj)
{
	DBG("");

	int w, h;
	Instance().Viewport(&w, &h);

	std::srand(std::time(0)); // use current time as seed for random generator

	float x = 0.0;
	for (int i = 0; i < 2000; ++i) {

		x = (i - 1000.0)/100.0;
		triangleVectices[i].x = x;
		triangleVectices[i].y = (float)std::rand() / RAND_MAX;
	}

	std::clock_t begin = std::clock();

	glViewport(0, 0, w, h);

	glClearColor(0.0f, 0.0f, 0.0f, 0.2f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint vbo;
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof triangleVectices, triangleVectices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(Instance().AttrPosition());
	glVertexAttribPointer(Instance().AttrPosition(), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINE_STRIP, 0, ARRAY_SIZE(triangleVectices));

	glDisableVertexAttribArray(Instance().AttrPosition());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	DBG("BENCHMARK: %f", (double)(std::clock() - begin)/CLOCKS_PER_SEC);
}

void MainView::GLResizeCb(Evas_Object *obj)
{
	DBG("");

	int w, h;

	elm_glview_size_get(obj, &w, &h);
	glViewport(0, 0, w, h);

	Instance().SetViewport(w, h);
}

void MainView::GLDelCb(Evas_Object *obj)
{
	DBG("");
}

void MainView::CreateTrace()
{
	if (gl_view_)
		return;

	gl_view_ = elm_glview_add(layout_);

	if (gl_view_) {

		DBG("Set elm_gl");

		ELEMENTARY_GLVIEW_GLOBAL_USE(gl_view_);

		elm_glview_init_func_set(gl_view_, GLInitCb);
		elm_glview_render_func_set(gl_view_, GLRenderCb);
		elm_glview_resize_func_set(gl_view_, GLResizeCb);
		elm_glview_del_func_set(gl_view_, GLDelCb);

		elm_glview_render_policy_set(gl_view_, ELM_GLVIEW_RENDER_POLICY_ALWAYS);
		elm_glview_resize_policy_set(gl_view_, ELM_GLVIEW_RESIZE_POLICY_RECREATE);
		elm_glview_mode_set(gl_view_, ELM_GLVIEW_ALPHA);

		elm_object_part_content_set(layout_, "trace", gl_view_);

		evas_object_size_hint_weight_set(gl_view_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(gl_view_, EVAS_HINT_FILL, EVAS_HINT_FILL);

		evas_object_show(gl_view_);
	}

	ecore_animator_frametime_set(0.03);
	if (!animator_)
		animator_ = ecore_animator_add(AnimateCb, this);
}

void MainView::SetYOffset(int offset)
{
	YOffset_ = offset;
}

}
