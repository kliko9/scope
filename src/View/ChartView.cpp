#include <cstdlib>
#include <ctime>

#include "View/ChartView.h"
#include "Log.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

namespace view {

ELEMENTARY_GLVIEW_GLOBAL_DEFINE();

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
	"	gl_FragColor = vec4(1.0, 1.0, 0.2, 1.0);\n"
	"}\n"
	;

struct Point {
	GLfloat x;
	GLfloat y;
};

Point triangleVectices[2000];

Eina_Bool ChartView::AnimateCb(void *data)
{
	ChartView *view = static_cast<ChartView *>(data);

	elm_glview_changed_set(view->gl_view_);

	return EINA_TRUE;
}

bool ChartView::GLInitShaders()
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

unsigned short ChartView::AttrPosition()
{
	return attrPosition;
}

void ChartView::GLInitCb(Evas_Object *obj)
{
	DBG("Init gl");

	ChartView::Instance().GLInitShaders();
}

void ChartView::GLRenderCb(Evas_Object *obj)
{
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

	//DBG("BENCHMARK: %f", (double)(std::clock() - begin)/CLOCKS_PER_SEC);
}

void ChartView::GLResizeCb(Evas_Object *obj)
{
	DBG("");

	int w, h;

	elm_glview_size_get(obj, &w, &h);
	glViewport(0, 0, w, h);

	Instance().SetViewport(w, h);
}

void ChartView::GLDelCb(Evas_Object *obj)
{
	DBG("");
}

void ChartView::SetViewport(int w, int h)
{
	viewport_h = h;
	viewport_w = w;
}

void ChartView::Viewport(int *w, int *h)
{
	*h = viewport_h;
	*w = viewport_w;
}

Evas_Object *ChartView::CreateContent(Evas_Object *parent) {

	if (gl_view_)
		return gl_view_;

	gl_view_ = elm_glview_add(parent);

	ELEMENTARY_GLVIEW_GLOBAL_USE(gl_view_);

	elm_glview_init_func_set(gl_view_, GLInitCb);
	elm_glview_render_func_set(gl_view_, GLRenderCb);
	elm_glview_resize_func_set(gl_view_, GLResizeCb);
	elm_glview_del_func_set(gl_view_, GLDelCb);

	elm_glview_render_policy_set(gl_view_, ELM_GLVIEW_RENDER_POLICY_ALWAYS);
	elm_glview_resize_policy_set(gl_view_, ELM_GLVIEW_RESIZE_POLICY_RECREATE);
	elm_glview_mode_set(gl_view_, ELM_GLVIEW_ALPHA);

	evas_object_size_hint_weight_set(gl_view_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(gl_view_, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_show(gl_view_);

	ecore_animator_frametime_set(0.03);
	if (!animator_)
		animator_ = ecore_animator_add(AnimateCb, this);

	return gl_view_;
}

ChartView::ChartView()
{

}

ChartView::~ChartView()
{

}

ChartView &ChartView::Instance()
{
	static ChartView instance;
	return instance;
}


} //namespace view
