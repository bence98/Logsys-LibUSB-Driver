#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include "logsys/usb.h"
#include "logsys/usb.private.h"

libusb_device_handle* logsys_usb_open(libusb_context* ctx, libusb_device* pDev){
	libusb_device_handle* dev;
	if(pDev==NULL) dev=libusb_open_device_with_vid_pid(ctx, LOGSYS_VID, LOGSYS_PID);
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

bool logsys_hotplug_enable(libusb_context* ctx, libusb_hotplug_event evt, libusb_hotplug_callback_fn cb, void* userdata, libusb_hotplug_callback_handle* hndl){
	int resp=libusb_hotplug_register_callback(ctx, evt, 0, LOGSYS_VID, LOGSYS_PID, LIBUSB_HOTPLUG_MATCH_ANY, cb, userdata, hndl);
	return resp==LIBUSB_SUCCESS;
}
