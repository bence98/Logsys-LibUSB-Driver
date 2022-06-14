#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include "logsys/control.h"
#include "logsys/usb.private.h" 

int logsys_get_status(libusb_device_handle* dev, LogsysStatus* data){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 1, 0, 0, (char*)data, sizeof(LogsysStatus), 0);
}

int logsys_clk_status(libusb_device_handle* dev, LogsysClkStatus* data){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, 0, 0x02, (char*)data, sizeof(LogsysClkStatus), 0);
}

int logsys_clk_start(libusb_device_handle* dev, int freqHz, bool* success){
	LogsysClkStatus status=logsys_create_clk_status(freqHz, 2);
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, TO_WORD(status.periodRegH, status.periodRegL), status.prescaler<<8, (char*)success, 1, 0);
}

int logsys_clk_stop(libusb_device_handle* dev, bool* was_running){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, 0, 0x0001, (char*)was_running, 1, 0);
}

int logsys_set_reset(libusb_device_handle* dev, bool reset, bool* success){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 5, 0, reset, (char*)success, 1, 0);
}

int logsys_get_reset(libusb_device_handle* dev, bool* reset){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 5, 0, 0x0002, (char*)reset, 1, 0);
}

int logsys_get_pwr_limit(libusb_device_handle* dev, LogsysPwrLimit* limit){
	char data;
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  2, 0x0100, 0, &data, 1, 0);
	if(resp>0) *limit=data;
	return resp;
}

int logsys_set_pwr_limit(libusb_device_handle* dev, LogsysPwrLimit limit){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 2, 0x0100|(char)limit, 0, NULL, 0, 0);
}

int logsys_get_pwr_corr(libusb_device_handle* dev, char* buf){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  2, 0x0200, 0, buf, 21, 0);
}

int logsys_set_vcc(libusb_device_handle* dev, bool vcc){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 2, vcc, 0, NULL, 0, 0);
}

int logsys_get_vcc(libusb_device_handle* dev, bool* vcc){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 2, 0, 0, (char*)vcc, 1, 0);
}

int logsys_get_rev_curr(libusb_device_handle* dev, double* mAmps){
	char data[2];
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 2, 0x0300, 0, data, 2, 0);
	if(resp<0) return resp;
	*mAmps=TO_WORD(data[1], data[0])*2560/(512*200*.2);
	return 1;
}

int logsys_set_rev_curr(libusb_device_handle* dev, double mAmps){
	short data=(mAmps*512*200*.2)/2560;
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 2, 0x0300, data, NULL, 0, 0);
}

int logsys_get_active_func(libusb_device_handle* dev, LogsysFunction* func, bool* active){
	char data[2];
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 3, 0, 0, data, 2, 0);
	if(resp>=0){
		if(func)   *func=data[0];
		if(active) *active=data[1];
	}
	return resp;
}

int logsys_fw_get_ver(libusb_device_handle* dev, LogsysFirmwareVersion* func){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 8, 0, 0, (char*)func, sizeof(LogsysFirmwareVersion), 0);
}

__attribute__((weak))
int logsys_fw_update(libusb_device_handle* dev){
	/* I won't give an implementation here. Students don't need to call this function;
	 * faculty members may apply for an "unlocked" version of the driver */
	return LIBUSB_ERROR_ACCESS;
}
