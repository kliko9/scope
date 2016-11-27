#include "Controller/MainController.h"
#include "Log.h"

namespace controller {

MainController &MainController::GetInstance() {
	static MainController controller;
	return controller;
}

MainController::MainController() {

}

void MainController::Init() {

	DBG("Main controller initialization");
}

} // namespace controller
