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

/** Serial I/O FizzBuzz demo */

bool run=true;

void sigint_cb(int i){
	printf("Quitting...\n");
	run=false;
}

void pins(LogsysSerialLines pins){
	if(pins.rising&SCLK)
		printf("C");
	else
		printf(" ");
	if(pins.rising&MISO)
		printf("I");
	else
		printf(" ");
	if(pins.rising&MOSI)
		printf("O");
	else
		printf(" ");
	if(pins.rising&RESET)
		printf("R");
	else
		printf(" ");
	if(pins.falling&SCLK)
		printf("c");
	else
		printf(" ");
	if(pins.falling&MISO)
		printf("i");
	else
		printf(" ");
	if(pins.falling&MOSI)
		printf("o");
	else
		printf(" ");
	if(pins.falling&RESET)
		printf("r");
	else
		printf(" ");
	printf("\n");
}

int main(void){
	printf("Logsys Serial IO test.\nExit with CTRL+c\n");
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
	
	logsys_serial_begin(logsys, &run);
	
	int x=0;
	while(run){
		printf("\tx=%d\n", x);
		LogsysSerialLines in, out;
		out.rising=0;
		out.falling=0;
		
		if(x%3==0){
			out.rising|=MOSI;
			out.falling|=MOSI;
		}
		if(x%5==0){
			out.rising|=RESET;
			out.falling|=RESET;
		}
		
		int res=logsys_serial_send(logsys, out, &in);
		if(res<0) fprintf(stderr, "Error: %d\n", res);
		pins(in);
		x++;
		usleep(10000);
	}
	printf("Exiting...\n");
	logsys_serial_end(logsys);
	logsys_usb_close(logsys);
	logsys_usb_end();
	return 0;
}
