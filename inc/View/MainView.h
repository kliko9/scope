#pragma once

#include <vector>

#include <Evas_GL.h>
#include <Elementary.h>
#include <cairo.h>

namespace view {

	class MainView {
	public:
		MainView();
		~MainView();

		Evas_Object *GetEvasObject();

		void SetYOffset(int offset);
		void SetBuffer(std::vector<double> &buffer);
	private:
		Evas_Object *win_;
		Evas_Object *conformant_;
		Evas_Object *layout_;

		Evas_Object *grid_ = nullptr;
		Evas_Object *trace_ = nullptr;

		cairo_device_t *cairo_device_ = nullptr;
		cairo_t *cairo_trace_ = nullptr;
		cairo_surface_t *surface_trace_ = nullptr;

		Evas_GL *evas_gl_ = nullptr;
		Evas_GL_Config *evas_gl_config_ = nullptr;
		Evas_GL_Surface *evas_gl_surface_ = nullptr;
		Evas_GL_Context *evas_gl_context_ = nullptr;
		Evas_Native_Surface ns_;

		Ecore_Animator *animator_ = nullptr;

		std::vector<double> buffer_;

		int lyW_, lyH_, lyX_, lyY_;
		int YOffset_ = 300;

		void CreateContent();
		void CreateMenu();
		void CreateBg();
		void CreateTrace();
		void CreateGrid(cairo_t *cairo, cairo_surface_t *surface);
		void CreateXYAxis(cairo_t *cairo, cairo_surface_t *surface);
		Evas_Object *CreateMenuButton(const char* name);
		static void LayoutResizeCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
		static void ButtonClickedCb(void *data, Evas_Object *obj, void *event_info);
		static void WinDeleteRequestCb(void *data, Evas_Object *obj, void *event_info);
		static void CairoDrawCb(void *data, Evas_Object *obj);
		static Eina_Bool AnimateCb(void *data);

	};
}
