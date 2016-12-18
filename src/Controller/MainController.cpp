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
{
	bluetooth_.RegisterSignal(model::Bluetooth::SignalType::BT_SIGNAL_DATA_RECEIVED,
			[this](void *data){this->BtDataReceive(data);});

	data_.SetBuffer(view_.Buffer(), view_.BufferSize());
}

void MainController::Init()
{
	DBG("Main controller initialization");
}

void MainController::BtDataReceive(void *data)
{
	std::clock_t begin = std::clock();

	if (!data) {
		return;
	}

	bt_socket_received_data_s *receivedData = static_cast<bt_socket_received_data_s *>(data);

	data_.Interpret(receivedData->data, receivedData->data_size);
}

} // namespace controller
