#pragma once

#include <bluetooth.h>
#include <vector>
#include <functional>

namespace model {

	class Bluetooth {
	public:

		enum class SignalType {
			BT_SIGNAL_DATA_RECEIVED,
			MAX
		};

		Bluetooth();
		virtual ~Bluetooth();

		void RegisterSignal(SignalType type, std::function<void(void *)> cb);
		void UnregisterSignal(SignalType type);
	private:
		const char *uuid = "00001101-0000-1000-8000-00805F9B34FB";
		int deviceSocketFD = -1;
		int serverSocketFD = -1;

		std::vector<std::function<void(void *)>> callbacks_
			= std::vector<std::function<void(void *)>>((int)SignalType::MAX);

		void EmitSignal(SignalType type, void *data);

		bool LaunchApp();
		bool CreateSocket();

		static void AdapterStateChangedCb(int result,
				bt_adapter_state_e adapterState,
				void *userData);
		static void BondCreatedCb(int result,
				bt_device_info_s *device_info,
				void *user_data);
		static void SocketConnectionStateChangedCb(int result,
				bt_socket_connection_state_e connection_state,
				bt_socket_connection_s *connection, void *user_data);
		static void DataReceivedCb(bt_socket_received_data_s *data, void *userData);
	};

}
