#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "logsys-usb.h"

bool run=true;

void sigint_cb(int i){
	printf("Quitting...\n");
	run=false;
}

int logsys_hp_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event evt_type, void *just_null){
	if(evt_type==LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED){
		printf("Logsys connected\n");
	}else{ //LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT
		printf("Logsys disconnected\n");
	}
	return 0;
}

int main(void){
	printf("Hotplugging test, press Ctrl+C to quit\n");
	signal(SIGINT, sigint_cb);
	if(!logsys_usb_init()){
		fprintf(stderr, "LibUSB error\n");
		return 1;
	}
	
	libusb_hotplug_callback_handle hndl;
	if(!logsys_hotplug_enable(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, logsys_hp_callback, &hndl)){
		fprintf(stderr, "Hotplug listen error\n");
		logsys_usb_end();
		return 3;
	}
	
	while(run){
		libusb_handle_events_completed(NULL, NULL);
		sleep(1);
	}
	libusb_hotplug_deregister_callback(NULL, hndl);
	return 0;
}
