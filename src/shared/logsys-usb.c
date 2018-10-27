#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdlib.h>
#include "logsys-usb.h"

bool logsys_was_init=false;

bool logsys_usb_init(){
	if(libusb_init(NULL)!=0) return false;
	return logsys_was_init=true;
}

bool logsys_usb_end(){
	libusb_exit(NULL);
	logsys_was_init=false;
	return true;
}

libusb_device_handle* logsys_usb_open(libusb_device* pDev){
	if(!logsys_was_init&&!logsys_usb_init())
		return NULL;
	libusb_device_handle* dev;
	if(pDev==NULL) dev=libusb_open_device_with_vid_pid(NULL, LOGSYS_VID, LOGSYS_PID);
	else libusb_open(pDev, &dev);
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
	if(!logsys_was_init&&!logsys_usb_init())
		return false;
	int resp=libusb_hotplug_register_callback(NULL, evt, 0, LOGSYS_VID, LOGSYS_PID, LIBUSB_HOTPLUG_MATCH_ANY, cb, NULL, hndl);
	return resp==LIBUSB_SUCCESS;
}

int logsys_tx_get_status(libusb_device_handle* dev, LogsysStatus* data){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 1, 0, 0, (char*)data, sizeof(LogsysStatus), 0);
}

int logsys_tx_clk_status(libusb_device_handle* dev, LogsysClkStatus* data){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, 0, 0x02, (char*)data, sizeof(LogsysClkStatus), 0);
}

int logsys_clk_start(libusb_device_handle* dev, int freqKHz, bool* success){
	LogsysClkStatus status=logsys_create_clk_status(freqKHz*1000.0, 2);
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, status.periodRegL|(status.periodRegH<<8), status.prescaler<<8, (char*)success, 1, 0);
}

int logsys_clk_stop(libusb_device_handle* dev, bool* was_running){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 4, 0, 0x0001, (char*)was_running, 1, 0);
}

int logsys_tx_set_reset(libusb_device_handle* dev, bool reset, /*out*/bool* success){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 5, 0, reset, (char*)success, 1, 0);
}

int logsys_tx_get_reset(libusb_device_handle* dev, /*out*/bool* reset){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 5, 0, 0x0002, (char*)reset, 1, 0);
}

int logsys_tx_get_pwr_limit(libusb_device_handle* dev, LogsysPwrLimit* limit){
	char data;
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  2, 0x0100, 0, &data, 1, 0);
	if(resp>0) *limit=data;
	return resp;
}

int logsys_tx_set_pwr_limit(libusb_device_handle* dev, LogsysPwrLimit limit){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 2, 0x0100|(char)limit, 0, NULL, 0, 0);
}

int logsys_tx_get_pwr_corr(libusb_device_handle* dev, char* buf){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  2, 0x0200, 0, buf, 21, 0);
}

int logsys_tx_set_vcc(libusb_device_handle* dev, bool vcc){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 2, vcc, 0, NULL, 0, 0);
}

int logsys_tx_get_vcc(libusb_device_handle* dev, bool* vcc){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 2, 0, 0, (char*)vcc, 1, 0);
}

int logsys_tx_get_rev_curr(libusb_device_handle* dev, double* mAmps){
	char data[2];
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 2, 0x0300, 0, data, 2, 0);
	if(resp<0) return resp;
	*mAmps=(data[1]<<8|data[0])*2560/(512*200*.2);
	return 1;
}

int logsys_tx_set_rev_curr(libusb_device_handle* dev, double mAmps){
	short data=(mAmps*512*200*.2)/2560;
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 2, 0x0300, data, NULL, 0, 0);
}

int logsys_tx_get_active_func(libusb_device_handle* dev, LogsysFunction* func){
	char data;
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 3, 0, 0, &data, 1, 0);
	if(resp>0) *func=data;
	return resp;
}

int logsys_tx_jtag_begin(libusb_device_handle* dev, LogsysJtagMode mode, /*out*/bool* ready){
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  16, mode, 0, (char*)ready, 1, 0);
	if(resp<0) return resp;
	if(!ready) return -1;
	
	//now we un-halt two endpoints
	libusb_clear_halt(dev, LOGSYS_OUT_EP1);
	libusb_clear_halt(dev, LOGSYS_IN_EP2);
	
	//we begin JTAG transmission?
	resp=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 18, 0, 0, NULL, 0, 0);
	if(resp<0) return resp;
	//we read 2 bytes
	//NOTE to self: it looks like `logsys_tx_get_active_func()`, except the wLength
	char tmp[2];
	resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  3, 0, 0, tmp, 2, 0);
	//this was in the disassembly of L-GUI, no clue why
	*ready=*ready&&tmp[0]==1&&tmp[1]!=0;
	return resp;
}

int logsys_tx_jtag_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 17, 0, 0, NULL, 0, 0);
}

int logsys_tx_serial_begin(libusb_device_handle* dev, bool* success){
	LogsysClkStatus status=logsys_create_clk_status(10, 4);
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 96, status.periodRegL|(status.periodRegH<<8), status.prescaler, (char*)success, 1, 0);
	if(resp<0) return resp;
	if(*success){
		resp=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 98, 0, 0, NULL, 0, 0);
	}
	return resp;
}

int logsys_tx_serial_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 97, 0, 0, NULL, 0, 0);
}

int logsys_jtag_scan(libusb_device_handle* dev, /*out*/uint32_t jtag_devs[], int max_devs, /*out*/int* found_devs){
	char req_preamb[]={0x87, 0xff, 0x83, 0x02},
		 req_get_id[]={0x07, 0xff, 0x07, 0xff, 0x07, 0xff, 0x07, 0xff},
		 req_postamb[]={0x87, 0xff},
		 read[8];
	
	int resp=libusb_bulk_transfer(dev, LOGSYS_OUT_EP1, req_preamb, sizeof(req_preamb), NULL, 0);
	if(resp<0) return resp;
	resp=libusb_bulk_transfer(dev, LOGSYS_IN_EP2, read, sizeof(req_preamb), NULL, 0);
	if(resp<0) return resp;
	
	for(*found_devs=0;*found_devs<max_devs;*found_devs++){
		resp=libusb_bulk_transfer(dev, LOGSYS_OUT_EP1, req_get_id, sizeof(req_get_id), NULL, 0);
		if(resp<0) break;
		resp=libusb_bulk_transfer(dev, LOGSYS_IN_EP2, read, sizeof(req_get_id), NULL, 0);
		if(resp<0) break;
		uint32_t id=read[0]|(read[2]<<8)|(read[4]<<16)|(read[6]<<24);
		if(id==0||id==-1) break;
		jtag_devs[*found_devs]=id;
	}
	resp=libusb_bulk_transfer(dev, LOGSYS_OUT_EP1, req_postamb, sizeof(req_postamb), NULL, 0);
	if(resp<0) return resp;
	resp=libusb_bulk_transfer(dev, LOGSYS_IN_EP2, read, sizeof(req_postamb), NULL, 0);
	return resp;
}

int logsys_jtag_get_mode(libusb_device_handle* dev, /*out*/LogsysJtagMode* mode){
	char tmp;
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 19, 0, 0, &tmp, 1, 0);
	if(resp==LIBUSB_SUCCESS) *mode=tmp;
	return resp;
}

int logsys_jtag_set_mode(libusb_device_handle* dev, LogsysJtagMode mode){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 19, mode, 0, NULL, 0, 0);
}

int logsys_jtag_check_error(libusb_device_handle* dev, /*out*/bool* error){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  20, 0, 0, (char*)error, 1, 0);
}
