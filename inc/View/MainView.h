#pragma once

#include <vector>

#include <Elementary_GL_Helpers.h>
#include <Elementary.h>
#include <cairo.h>

#include "ChartView.h"

namespace view {

	class MainView {
	public:
		~MainView();
		MainView();

		Evas_Object *GetEvasObject();

		void SetYOffset(int offset);
		void SetBuffer(std::vector<float> &buffer);
	private:
		ChartView &chart_;

		Evas_Object *win_;
		Evas_Object *conformant_;
		Evas_Object *layout_;

		Evas_Object *grid_ = nullptr;
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

		static void LayoutResizeCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
		static void ButtonClickedCb(void *data, Evas_Object *obj, void *event_info);
		static void WinDeleteRequestCb(void *data, Evas_Object *obj, void *event_info);
	};
}
