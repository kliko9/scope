#include "Controller/MainController.h"
#include "Log.h"

namespace controller {

MainController &MainController::GetInstance() {
	static MainController controller;
	return controller;
}

MainController::MainController() {

	bluetooth_.RegisterSignal(model::Bluetooth::SignalType::BT_SIGNAL_DATA_RECEIVED,
			[this](void *data){this->BtDataReceive(data);});
}

void MainController::Init() {

	DBG("Main controller initialization");
}

void MainController::BtDataReceive(void *data) {

	if (!data) {
		return;
	}

	char *text = static_cast<char *>(data);

	DBG("BtDataReceive controller cb: %s", text);

}

} // namespace controller
