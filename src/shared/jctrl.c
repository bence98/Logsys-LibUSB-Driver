#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include "logsys/common.h"
#include "logsys/jconf.h"
#include "logsys/usb.private.h"

int logsys_jtag_begin(libusb_device_handle* dev, LogsysJtagMode mode, bool* ready){
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
	//NOTE to self: it looks like `logsys_get_active_func()`, except the wLength
	char tmp[2];
	resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  3, 0, 0, tmp, 2, 0);
	//this was in the disassembly of L-GUI, no clue why
	//edit: modified as it wouldn't start in MODE_CMP mode
	*ready=tmp[0]==mode&&tmp[1]!=0;
	return resp;
}

int logsys_jtag_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 17, 0, 0, NULL, 0, 0);
}

int logsys_jtag_scan(libusb_device_handle* dev, uint32_t jtag_devs[], int max_devs, int* found_devs){
	char req_preamb[]={0x87, 0xff, 0x83, 0x02},
		 req_get_id[]={0x07, 0xff, 0x07, 0xff, 0x07, 0xff, 0x07, 0xff},
		 req_postamb[]={0x87, 0xff},
		 read[8];
	
	int resp=libusb_bulk_transfer(dev, LOGSYS_OUT_EP1, req_preamb, sizeof(req_preamb), NULL, 0);
	if(resp<0) return resp;
	resp=libusb_bulk_transfer(dev, LOGSYS_IN_EP2, read, sizeof(req_preamb), NULL, 0);
	if(resp<0) return resp;
	
	int devid;
	for(devid=0;devid<max_devs;devid++){
		resp=libusb_bulk_transfer(dev, LOGSYS_OUT_EP1, req_get_id, sizeof(req_get_id), NULL, 0);
		if(resp<0) break;
		resp=libusb_bulk_transfer(dev, LOGSYS_IN_EP2, read, sizeof(req_get_id), NULL, 0);
		if(resp<0) break;
		uint32_t id=TO_DWORD(read[6], read[4], read[2], read[0]);
		if(id==0||id==-1) break;
		jtag_devs[devid]=id;
	}
	if(resp<0) return resp;
	
	*found_devs=devid;
	resp=libusb_bulk_transfer(dev, LOGSYS_OUT_EP1, req_postamb, sizeof(req_postamb), NULL, 0);
	if(resp<0) return resp;
	resp=libusb_bulk_transfer(dev, LOGSYS_IN_EP2, read, sizeof(req_postamb), NULL, 0);
	return resp;
}

int logsys_jtag_get_mode(libusb_device_handle* dev, LogsysJtagMode* mode){
	char tmp;
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 19, 0, 0, &tmp, 1, 0);
	if(resp==LIBUSB_SUCCESS) *mode=tmp;
	return resp;
}

int logsys_jtag_set_mode(libusb_device_handle* dev, LogsysJtagMode mode){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 19, mode, 0, NULL, 0, 0);
}

int logsys_jtag_check_error(libusb_device_handle* dev, bool* error){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  20, 0, 0, (char*)error, 1, 0);
}
