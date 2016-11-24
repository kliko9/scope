#include <Elementary.h>
#include <cairo.h>

namespace view {

	class MainView {
	public:
		MainView();
		Evas_Object *GetEvasObject();
	private:
		Evas_Object *win_;
		Evas_Object *conformant_;
		Evas_Object *layout_;

		Evas_Object *grid1_ = nullptr;
		Evas_Object *grid2_ = nullptr;

		void CreateContent();
		void CreateGrid();
		static void LayoutResizeCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
	};
}
