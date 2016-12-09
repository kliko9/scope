#include <ctime>

#include "Controller/MainController.h"
#include "Log.h"

namespace controller {

MainController &MainController::GetInstance()
{
	static MainController controller;
	return controller;
}

MainController::MainController()
	: view_(view::MainView::Instance())
{
	bluetooth_.RegisterSignal(model::Bluetooth::SignalType::BT_SIGNAL_DATA_RECEIVED,
			[this](void *data){this->BtDataReceive(data);});
}

void MainController::Init()
{
	DBG("Main controller initialization");
}

void MainController::BtDataReceive(void *data)
{

	DBG("DATA RECEIVED");
	std::clock_t receive;
	std::clock_t trace;

	std::clock_t begin = std::clock();

	if (!data) {
		return;
	}

	char *receivedData = static_cast<char *>(data);

	std::vector<float> buffer = data_.Interpret(receivedData);
	if (buffer.size() < 1) {
		ERR("Invalid data");
		return;
	}

	receive = std::clock();

	view_.SetBuffer(buffer);

	trace = std::clock();

	DBG("BENCHMARK:");
	DBG("receiving: %f", (double)(receive - begin)/CLOCKS_PER_SEC);
	DBG("drawing: %f", (double)(trace - receive)/CLOCKS_PER_SEC);
}

} // namespace controller
