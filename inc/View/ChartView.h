#pragma once

#include <Elementary_GL_Helpers.h>
#include <Elementary.h>

namespace view {

	class ChartView {
	public:
		ChartView();
		virtual ~ChartView();

		static ChartView &Instance();

		unsigned short AttrPosition();
		void Viewport(int *w, int *h);
		void SetViewport(int w, int h);
		Evas_Object *CreateContent(Evas_Object *parent);

	private:
		Evas_Object *gl_view_ = nullptr;

		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint program;
		GLuint attrPosition;

		int viewport_h, viewport_w;

		Ecore_Animator *animator_ = nullptr;

		bool GLInitShaders();

		static void GLInitCb(Evas_Object *obj);
		static void GLRenderCb(Evas_Object *obj);
		static void GLResizeCb(Evas_Object *obj);
		static void GLDelCb(Evas_Object *obj);
		static Eina_Bool AnimateCb(void *data);
	};
}

