#include "View/MainView.h"

namespace controller {

	class MainController {
	public:
		static MainController &GetInstance();
		void Init();

	private:
		view::MainView view;

		MainController();
		MainController(const MainController &) = delete;
		void operator =(const MainController &) = delete;
	};
}
