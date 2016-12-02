#include "View/MainView.h"
#include "Model/Bluetooth.h"
#include "Model/DataInterpreter.h"

namespace controller {

	class MainController {
	public:
		static MainController &GetInstance();
		void Init();

	private:
		view::MainView view_;

		model::Bluetooth bluetooth_;
		model::DataInterpreter data_;

		MainController();
		MainController(const MainController &) = delete;
		void operator =(const MainController &) = delete;

		void BtDataReceive(void *data);
	};
}
