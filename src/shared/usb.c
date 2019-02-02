#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "logsys/usb.h"
#include "logsys/serio.h"

/* interface numbers */
#define LOGSYS_IFN_DEV 0
#define LOGSYS_IFN_COM 1
/* endpoints: - interface0 */
/* non-interrupting function endpoints */
#define LOGSYS_OUT_EP1 0x01
#define LOGSYS_IN_EP2  0x82
/* interrupting function endpoints */
#define LOGSYS_OUT_EP3 0x03
#define LOGSYS_IN_EP4  0x84
/* - interface1 */
/* COM endpoints */
#define LOGSYS_OUT_EP5 0x05
#define LOGSYS_IN_EP6  0x86

#define LOGSYS_REQTYP_IN  0xc0
#define LOGSYS_REQTYP_OUT 0x40

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

int logsys_clk_start(libusb_device_handle* dev, int freqHz, bool* success){
	LogsysClkStatus status=logsys_create_clk_status(freqHz, 2);
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, TO_WORD(status.periodRegH, status.periodRegL), status.prescaler<<8, (char*)success, 1, 0);
}

int logsys_clk_stop(libusb_device_handle* dev, bool* was_running){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, 0, 0x0001, (char*)was_running, 1, 0);
}

int logsys_tx_set_reset(libusb_device_handle* dev, bool reset, bool* success){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 5, 0, reset, (char*)success, 1, 0);
}

int logsys_tx_get_reset(libusb_device_handle* dev, bool* reset){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 5, 0, 0x0002, (char*)reset, 1, 0);
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
	*mAmps=TO_WORD(data[1], data[0])*2560/(512*200*.2);
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

int logsys_tx_jtag_begin(libusb_device_handle* dev, LogsysJtagMode mode, bool* ready){
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
	//edit: modified as it wouldn't start in MODE_CMP mode
	*ready=tmp[0]==mode&&tmp[1]!=0;
	return resp;
}

int logsys_tx_jtag_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 17, 0, 0, NULL, 0, 0);
}

int logsys_tx_serial_begin(libusb_device_handle* dev, bool* success){
	LogsysClkStatus status=logsys_create_clk_status(10, 4);
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 96, TO_WORD(status.periodRegH, status.periodRegL), status.prescaler, (char*)success, 1, 0);
	if(resp<0) return resp;
	if(*success){
		resp=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 98, 0, 0, NULL, 0, 0);
	}
	return resp;
}

int logsys_tx_serial_change_clk(libusb_device_handle* dev, int freqHz){
	LogsysClkStatus status=logsys_create_clk_status(freqHz, 4);
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 99, TO_WORD(status.periodRegH, status.periodRegL), status.prescaler, NULL, 0, 0);
}

int logsys_tx_serial_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 97, 0, 0, NULL, 0, 0);
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

int logsys_serial_send(libusb_device_handle* dev, LogsysSerialLines send, LogsysSerialLines* recv){
	int res=libusb_bulk_transfer(dev, LOGSYS_OUT_EP3, (char*)&send, 2, NULL, 0);
	if(res<0) return res;
	return libusb_bulk_transfer(dev, LOGSYS_IN_EP4, (char*)recv, 2, NULL, 0);
}

int logsys_usart_get_caps(libusb_device_handle* dev, LogsysUsartCaps* capabilities){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 48, 0, 0, (char*)capabilities, sizeof(LogsysUsartCaps), 0);
}

int logsys_tx_usart_begin(libusb_device_handle* dev, unsigned int baud, bool usrt, LogsysUsartEncoding enc, LogsysUsartParity parity, bool* success){
	if(baud<=30||baud>2000000) return -2;
	int res=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 49, usrt, 0, (char*)success, 1, 0);
	if(res<0) return res;
	// Calculating wValue
	// It looks like a clock register with 1/8 prescaler
	static int mcuFreqHz=16000000; //16 MHz
	unsigned short wValue=(mcuFreqHz/8/baud)-1;
	res=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 53, wValue, 0, NULL, 0, 0);
	if(res<0) return res;
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 54, enc, parity, NULL, 0, 0);
}

int logsys_tx_usart_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 50, 0, 0, NULL, 0, 0);
}

int logsys_usart_getstr(libusb_device_handle* dev, char* buf, int maxlen, int* len, LogsysUsartStatus* status){
	char tmp[maxlen+1];
	int res=libusb_interrupt_transfer(dev, LOGSYS_IN_EP6, tmp, maxlen+1, len, 0);
	if(res>=0){
		memcpy(buf, tmp, --(*len));
		*status=tmp[*len];
	}
	return res;
}

int logsys_usart_putstr(libusb_device_handle* dev, char* buf, int len){
	return libusb_bulk_transfer(dev, LOGSYS_OUT_EP5, buf, len, NULL, 0);
}
