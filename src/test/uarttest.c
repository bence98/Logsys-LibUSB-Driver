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

bool run=true;

void sigint_cb(int i){
	printf("Quitting...\n");
	run=false;
}

int main(void){
	printf("Logsys USART test.\nExit with CTRL+c\n");
	signal(SIGINT, sigint_cb);
	if(libusb_init(NULL)!=0){
		fprintf(stderr, "LibUSB error\n");
		return 1;
	}
	
	libusb_device_handle* logsys=logsys_usb_open(NULL);
	if(logsys==NULL){
		fprintf(stderr, "USB open error\n");
		libusb_exit(NULL);
		return 2;
	}
	
	logsys_usart_begin(logsys, 115200, true, DATA_BITS_8, 0, &run);
	
	int x=0;
	char str[20]={0};
	while(run){
		int strlen=snprintf(str, sizeof(str), "x=%d\n", x);
		int res=logsys_usart_putstr(logsys, str, strlen);
		if(res<0) fprintf(stderr, "Error: %d\n", res);
		LogsysUsartStatus st;
		int len=0;
		res=logsys_usart_getstr(logsys, str, 19, &len, &st);
		if(res<0) fprintf(stderr, "Read error: %d\n", res);
		str[len]='\0';
		printf("[%d] Got %s\n", st, str);
		x++;
		usleep(10000);
	}
	printf("Exiting...\n");
	logsys_usart_end(logsys);
	logsys_usb_close(logsys);
	libusb_exit(NULL);
	return 0;
}
