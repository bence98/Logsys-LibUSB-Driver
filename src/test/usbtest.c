#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <string.h>
#include "logsys-usb.h"

void print_buf(char* buf, int len){
	for(int i=0;i<len;i++)
		fprintf(stderr, "%02X", buf[i]);
}

int main(void){
	printf("New\n");
	if(!logsys_usb_init()){
		fprintf(stderr, "LibUSB error\n");
		return 1;
	}
	
	libusb_device_handle* logsys=logsys_usb_open();
	if(logsys==NULL){
		fprintf(stderr, "USB open error\n");
		logsys_usb_end();
		return 2;
	}
	
	char cmd;
	while(scanf("%c", &cmd)>=1){
		int res;
		switch(cmd){
			case '1':
				fprintf(stderr, "REQ1\n");
				LogsysStatus status={0};
				res=logsys_tx_get_status(logsys, &status);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
				fprintf(stderr, "Got "); print_buf(&status, sizeof(status)); fprintf(stderr, "\n");
				break;
			case '4':
				fprintf(stderr, "REQ4\n");
				LogsysClkStatus clkStatus={0};
				res=logsys_tx_clk_status(logsys, &clkStatus);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
				fprintf(stderr, "Got "); print_buf(&clkStatus, sizeof(clkStatus)); fprintf(stderr, "\n");
				break;
			case '2':
				fprintf(stderr, "REQ2\n");
				char buf2[21];
				LogsysPwrLimit limit={0};
				res=logsys_tx_get_pwr_limit(logsys, &limit);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
				res=logsys_tx_get_pwr_corr(logsys, buf2);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
				fprintf(stderr, "Got %02X ", limit); print_buf(buf2, 21); fprintf(stderr, "\n");
				break;
			case '+':
				fprintf(stderr, "Vcc ON\n");
				res=logsys_tx_set_vcc(logsys, true);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
				break;
			case '-':
				fprintf(stderr, "Vcc OFF\n");
				res=logsys_tx_set_vcc(logsys, false);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
				break;
				case '?':
				fprintf(stderr, "JTAG scan\n");
				char jtag[2], num;
				res=logsys_tx_scan_jtag(logsys, &num, jtag);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
				fprintf(stderr, "Got %02X ", num); print_buf(jtag, 2); fprintf(stderr, "\n");
				break;
			case 'q':
				logsys_usb_close(logsys);
				logsys_usb_end();
				return 0;
		}
	}
}
