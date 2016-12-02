#include <app_control.h>

#include "Model/Bluetooth.h"
#include "Log.h"

namespace model {

Bluetooth::Bluetooth() {

	DBG("Bluetooth initialization");

	int ret = bt_initialize();
	if (ret != BT_ERROR_NONE) {
		DBG("bt_initialize failed: %s", get_error_message(ret));
		//abort();
		return;
	}

	bt_adapter_state_e btState;
	ret = bt_adapter_get_state(&btState);
	if (ret != BT_ERROR_NONE) {
		DBG("bt_initialize failed: %s", get_error_message(ret));
		//abort();
		return;
	}

	ret = bt_adapter_set_state_changed_cb(AdapterStateChangedCb, this);
	if (ret != BT_ERROR_NONE) {
		DBG("bt_adapter_set_state_changed_cb failed: %s", get_error_message(ret));
		//abort();
		return;
	}

	ret = bt_device_set_bond_created_cb(BondCreatedCb, this);
	if (ret != BT_ERROR_NONE) {
		DBG("bt_device_set_bond_created_cb failed: %s", get_error_message(ret));
		//abort();
		return;
	}

	if (btState != BT_ADAPTER_ENABLED) {
		if (!LaunchApp()) {
			DBG("Bluetooth initialization failed");
			//abort();
			return;
		}
	}

	CreateSocket();
}

Bluetooth::~Bluetooth() {
	DBG("");
	bt_deinitialize();
	DBG("");
}

bool Bluetooth::LaunchApp() {

	DBG("Bluetooth launch app");

	app_control_h appControl;

	int ret = app_control_create(&appControl);
	if (ret != APP_CONTROL_ERROR_NONE) {
		DBG("app_control_create failed: %s", get_error_message(ret));
		return false;
	}

	ret = app_control_set_app_id(appControl, "ug-bluetooth-efl");
	if (ret != APP_CONTROL_ERROR_NONE) {
		DBG("app_control_set_app_id failed: %s", get_error_message(ret));
		app_control_destroy(appControl);
		return false;
	}

	ret = app_control_send_launch_request(appControl, NULL, NULL);
	if (ret != APP_CONTROL_ERROR_NONE) {
		app_control_destroy(appControl);
		DBG("app_control_send_launch_request failed: %s", get_error_message(ret));
		return false;
	}

	app_control_destroy(appControl);

	return true;
}

void Bluetooth::BondCreatedCb(int result, bt_device_info_s *device_info, void *user_data) {

	if (device_info != NULL) {
		DBG("Bonded device info:");
		DBG("Remote adress: %s", device_info->remote_address);
		DBG("Remote name: %s", device_info->remote_name);
		DBG("Is bonded: %d", device_info->is_bonded);
		DBG("Is connected: %d", device_info->is_connected);
	} else
		DBG("No device info");
}

void Bluetooth::AdapterStateChangedCb(int result, bt_adapter_state_e adapterState, void *userData) {

	DBG("");

	/*
	 * Get state and search for bonded devices
	 */

	if (result != BT_ERROR_NONE) {
		ERR("BluetoothAdapterStateChangedCb failed");
		return;
	}
}

void Bluetooth::SocketConnectionStateChangedCb(int result, bt_socket_connection_state_e state,
                                     bt_socket_connection_s *connection, void *userData) {

	DBG("Socket connection state changed callback");

	Bluetooth *bt = static_cast<Bluetooth *>(userData);

	if (result != BT_ERROR_NONE) {
		DBG("SocketConnectionStateChangedCb failed");
		return;
	}

	if (state == BT_SOCKET_CONNECTED) {
		if (connection == NULL) {
			DBG("No connection data");
			return;
		}

		if (bt->serverSocketFD != -1)
			bt_socket_disconnect_rfcomm(bt->serverSocketFD);

		bt->serverSocketFD = connection->socket_fd;
	}
}

void Bluetooth::DataReceivedCb(bt_socket_received_data_s *data, void *userData) {

	Bluetooth *bt = static_cast<Bluetooth *>(userData);

	DBG("Received data:");
	DBG("%s", data->data);

	char reply[] = "Data received\n";

	bt_socket_send_data(bt->serverSocketFD, reply, sizeof(reply));

	bt->EmitSignal(SignalType::BT_SIGNAL_DATA_RECEIVED, data->data);
}

bool Bluetooth::CreateSocket() {

	DBG("Bluetooth creating socket");

	int ret = bt_socket_create_rfcomm(uuid, &deviceSocketFD);
	if (ret != BT_ERROR_NONE) {
		DBG("bt_socket_create_rfcomm failed: %s", get_error_message(ret));
		return false;
	}

	ret = bt_socket_set_connection_state_changed_cb(SocketConnectionStateChangedCb, this);
	if (ret != BT_ERROR_NONE) {
		bt_socket_destroy_rfcomm(deviceSocketFD);
		deviceSocketFD = -1;
		DBG("bt_socket_set_connection_state_changed_cb failed: %s", get_error_message(ret));
		return false;
	}

	ret = bt_socket_listen_and_accept_rfcomm(deviceSocketFD, 1);
	if (ret != BT_ERROR_NONE) {
		bt_socket_unset_connection_state_changed_cb();
		bt_socket_destroy_rfcomm(deviceSocketFD);
		deviceSocketFD = -1;
		DBG("bt_socket_set_connection_state_changed_cb failed: %s", get_error_message(ret));
		return false;
	}

	ret = bt_socket_set_data_received_cb(DataReceivedCb, this);
	if (ret != BT_ERROR_NONE) {
		bt_socket_unset_connection_state_changed_cb();
		bt_socket_destroy_rfcomm(deviceSocketFD);
		deviceSocketFD = -1;
		DBG("bt_socket_set_connection_state_changed_cb failed: %s", get_error_message(ret));
		return false;
	}

	return true;
}

void Bluetooth::RegisterSignal(SignalType type, std::function<void(void *)> cb)
{
	if (!cb) {
		ERR("Callback function == NULL");
		return;
	}

	callbacks_[(int)type] = cb;
}

void Bluetooth::UnregisterSignal(SignalType type)
{
	callbacks_[(int)type] = nullptr;
}

void Bluetooth::EmitSignal(SignalType type, void *data)
{
	if (callbacks_[(int)type])
		callbacks_[(int)type](data);
}

} //namespace model
