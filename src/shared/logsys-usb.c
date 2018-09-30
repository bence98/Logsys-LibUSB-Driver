#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdlib.h>
#include "logsys-usb.h"

bool libusb_was_init=false;

bool logsys_usb_init(){
	if(libusb_init(NULL)!=0) return false;
	return libusb_was_init=true;
}

bool logsys_usb_end(){
	libusb_exit(NULL);
	return true;
}

libusb_device_handle* logsys_usb_open(){
	if(!libusb_was_init&&!logsys_usb_init())
		return NULL;
	libusb_device_handle* dev=libusb_open_device_with_vid_pid(NULL, LOGSYS_VID, LOGSYS_PID);
	if(dev!=NULL){
		libusb_claim_interface(dev, LOGSYS_IFN_DEV);
		libusb_claim_interface(dev, LOGSYS_IFN_COM);
	}
	return dev;
}

void logsys_usb_close(libusb_device_handle* dev){
	libusb_release_interface(dev, LOGSYS_IFN_DEV);
	libusb_release_interface(dev, LOGSYS_IFN_COM);
	libusb_close(dev);
}

bool logsys_hotplug_enable(libusb_hotplug_event evt, libusb_hotplug_callback_fn cb, libusb_hotplug_callback_handle* hndl){
	if(!libusb_was_init&&!logsys_usb_init())
		return false;
	int resp=libusb_hotplug_register_callback(NULL, evt, 0, LOGSYS_VID, LOGSYS_PID, LIBUSB_HOTPLUG_MATCH_ANY, cb, NULL, hndl);
	return resp==LIBUSB_SUCCESS;
}

int logsys_tx_get_status(libusb_device_handle* dev, char* data){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 1, 0, 0, data, 12, 0);
}

int logsys_tx_clk(libusb_device_handle* dev, char* data){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, 0, 0x02, data, 1, 0);
}

int logsys_tx_pwr_limit(libusb_device_handle* dev, char* ch, char* buf){
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  2, 0x0100, 0, ch, 1, 0);
	if(resp<0) return resp;
	resp=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 2, 0x0100, 0, NULL, 0, 0);
	if(resp<0) return resp;
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  2, 0x0200, 0, buf, 21, 0);
}

int logsys_tx_set_vcc(libusb_device_handle* dev, bool vcc){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 2, vcc, 0, NULL, 0, 0);
}

int logsys_tx_scan_jtag(libusb_device_handle* dev, char* num_boards, char* jtag_dev){
	//so I have not the slightest clue here
	//but we read what appears to be a device count
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  16, 0x0001, 0, num_boards, 1, 0);
	if(resp<0) return resp;
	
	//now we un-halt two endpoints
	libusb_clear_halt(dev, 1);
	libusb_clear_halt(dev, 130);
	
	//we begin JTAG transmission?
	resp=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 18, 0, 0, NULL, 0, 0);
	if(resp<0) return resp;
	//we read 2 bytes
	resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  3, 0x0001, 0, jtag_dev, 2, 0);
	if(resp<0) return resp;
	//we end JTAG transmission?
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 17, 0, 0, NULL, 0, 0);
}
