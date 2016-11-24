#include "Controller/MainController.h"

namespace controller {

MainController &MainController::GetInstance() {
	static MainController controller;
	return controller;
}

MainController::MainController() {
}

void MainController::Init() {
}

} // namespace controller
