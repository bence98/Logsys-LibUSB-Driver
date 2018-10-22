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
	static double mcuFreqKHz=16000; //16 MHz
	static double prescaler[]={1.0, 1.0/8, 1.0/64, 1.0/256, 1.0/1024};
	
	double error=0, minError=freqKHz, newFreqKHz;
	short prescalerVal, perRegVal;
	
	for(int i=0;i<5;i++){
		int tryPerRegVal=(mcuFreqKHz*prescaler[i]/(2*freqKHz));
		if(tryPerRegVal<0||tryPerRegVal>65535) continue;
		newFreqKHz=mcuFreqKHz*prescaler[i]/(tryPerRegVal*2);
		error=abs(freqKHz-newFreqKHz);
		if(error<minError){
			minError=error;
			prescalerVal=i+1;
			perRegVal=tryPerRegVal-1;
		}
		if(error==0) break;
	}
	
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 4, perRegVal, prescalerVal<<8, (char*)success, 1, 0);
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

int logsys_jtag_begin(libusb_device_handle* dev, /*out*/bool* ready, /*out*/char* jtag_dev){
	//so I have not the slightest clue here
	//but I think this is a query to make sure the endpoint is ready
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  16, 0x0001, 0, (char*)ready, 1, 0);
	if(resp<0) return resp;
	if(!ready) return -1;
	
	//now we un-halt two endpoints
	libusb_clear_halt(dev, LOGSYS_OUT_EP1);
	libusb_clear_halt(dev, LOGSYS_IN_EP2);
	
	//we begin JTAG transmission?
	resp=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 18, 0, 0, NULL, 0, 0);
	if(resp<0) return resp;
	//we read 2 bytes
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  3, 0, 0, jtag_dev, 2, 0);
}

int logsys_jtag_end(libusb_device_handle* dev){
	//we end JTAG transmission?
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 17, 0, 0, NULL, 0, 0);
}

int logsys_jtag_scan(libusb_device_handle* dev){
	//TODO: there are more bulk transfers
	char tmp[]={0x87, 0xff, 0x83, 0x02};
	return libusb_bulk_transfer(dev, LOGSYS_OUT_EP1, tmp, 4, NULL, 0);
}

int logsys_jtag_get_mode(libusb_device_handle* dev, /*out*/char* mode){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 19, 0, 0, mode, 1, 0);
}

int logsys_jtag_set_mode(libusb_device_handle* dev, char mode){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 19, mode, 0, NULL, 0, 0);
}

int logsys_jtag_check_error(libusb_device_handle* dev, /*out*/bool* error){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN,  20, 0, 0, (char*)error, 1, 0);
}
