#include "View/MainView.h"
#include "Model/Bluetooth.h"

namespace controller {

	class MainController {
	public:
		static MainController &GetInstance();
		void Init();

	private:
		view::MainView view;
		model::Bluetooth bluetooth;

		MainController();
		MainController(const MainController &) = delete;
		void operator =(const MainController &) = delete;
	};
}
