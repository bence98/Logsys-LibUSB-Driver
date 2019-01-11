#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "logsys/usb.h"
#include "logsys/serio.h"
#include "logsys/status.h"

bool run=true, mode=false;

void sigint_cb(int i){
	printf("Quitting...\n");
	run=false;
}

int main(void){
	printf("Logsys USART test.\nExit with CTRL+c\n");
	signal(SIGINT, sigint_cb);
	if(!logsys_usb_init()){
		fprintf(stderr, "LibUSB error\n");
		return 1;
	}
	
	libusb_device_handle* logsys=logsys_usb_open(NULL);
	if(logsys==NULL){
		fprintf(stderr, "USB open error\n");
		logsys_usb_end();
		return 2;
	}
	
	logsys_tx_usart_begin(logsys, mode, &run);
	
	int x=0;
	char str[20]={0};
	while(run){
		sprintf(str, "x=%d\n", x);
		int res=logsys_usart_putstr(logsys, str, strlen(str));
		if(res<0) fprintf(stderr, "Error: %d\n", res);
		x++;
		usleep(10000);
	}
	printf("Exiting...\n");
	logsys_tx_usart_end(logsys);
	logsys_usb_close(logsys);
	logsys_usb_end();
	return 0;
}
