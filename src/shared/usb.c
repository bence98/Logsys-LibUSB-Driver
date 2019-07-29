#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include "logsys/usb.h"
#include "logsys/usb.private.h"

static bool logsys_was_init=false;

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
