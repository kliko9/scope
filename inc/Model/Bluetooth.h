#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <bluetooth.h>

namespace model {

	class Bluetooth {
	public:
		Bluetooth();
		virtual ~Bluetooth();
	private:
		const char *uuid = "00001101-0000-1000-8000-00805F9B34FB";
		int deviceSocketFD = -1;
		int serverSocketFD = -1;

		bool BluetoothLaunchApp();
		bool BluetoothCreateSocket();

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

#endif //BLUETOOTH_H
