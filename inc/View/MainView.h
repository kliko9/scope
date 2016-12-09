#pragma once

#include <vector>

#include <Elementary_GL_Helpers.h>
#include <Elementary.h>
#include <cairo.h>

namespace view {

	class MainView {
	public:
		~MainView();

		Evas_Object *GetEvasObject();
		static MainView &Instance();

		unsigned short AttrPosition();
		void Viewport(int *w, int *h);

		void SetYOffset(int offset);
		void SetBuffer(std::vector<float> &buffer);
		void SetViewport(int w, int h);
	private:
		MainView();
		MainView(const MainView &) = delete;
		MainView &operator=(const MainView &) = delete;

		Evas_Object *win_;
		Evas_Object *conformant_;
		Evas_Object *layout_;

		Evas_Object *grid_ = nullptr;
		Evas_Object *gl_view_ = nullptr;

		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint program;
		GLuint attrPosition;
		int viewport_h, viewport_w;

		Ecore_Animator *animator_ = nullptr;

		std::vector<float> buffer_;

		int lyW_, lyH_, lyX_, lyY_;
		int YOffset_ = 300;

		void CreateContent();
		void CreateMenu();
		void CreateBg();
		void CreateTrace();
		void CreateGrid(cairo_t *cairo, cairo_surface_t *surface);
		void CreateXYAxis(cairo_t *cairo, cairo_surface_t *surface);
		Evas_Object *CreateMenuButton(const char* name);

		bool GLInitShaders();

		static void LayoutResizeCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
		static void ButtonClickedCb(void *data, Evas_Object *obj, void *event_info);
		static void WinDeleteRequestCb(void *data, Evas_Object *obj, void *event_info);
		static Eina_Bool AnimateCb(void *data);

		static void GLInitCb(Evas_Object *obj);
		static void GLRenderCb(Evas_Object *obj);
		static void GLResizeCb(Evas_Object *obj);
		static void GLDelCb(Evas_Object *obj);

	};
}
