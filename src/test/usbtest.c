#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <string.h>
#include "logsys-usb.h"
#include "logsys-status.h"

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
	
	libusb_device_handle* logsys=logsys_usb_open(NULL);
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
				fprintf(stderr, "Get status\n");
				LogsysStatus status={0};
				res=logsys_tx_get_status(logsys, &status);
				if(res<0){
					fprintf(stderr, "Failed! %d\n", res);
					break;
				}
				
				fprintf(stderr, " VCC | V_j | Vio |  I  |*+-|J12RSNI\n");
				fprintf(stderr, "%5.2f|%5.2f|%5.2f|%5.2f|%c%c%c|%c%c%c%c%c%c%c\n",
					logsys_get_vcc_out(status),
					logsys_get_vjtag_out(status),
					logsys_get_vio_out(status),
					logsys_get_current_ma(status),
					logsys_is_vcc(status)?'.':' ',
					logsys_is_overcurr(status)?'.':' ',
					logsys_is_revcurr(status)?'.':' ',
					
					logsys_is_jtag_used(status)?'.':' ',
					logsys_is_clk1_used(status)?'.':' ',
					logsys_is_clk2_used(status)?'.':' ',
					logsys_is_rst_used(status)?'.':' ',
					logsys_is_sio_used(status)?'.':' ',
					logsys_is_end_ni_used(status)?'.':' ',
					logsys_is_end_int_used(status)?'.':' '
				);
				break;
			case '4':
				fprintf(stderr, "Clock status:\n");
				LogsysClkStatus clkStatus={0};
				res=logsys_tx_clk_status(logsys, &clkStatus);
				if(res<0){
					fprintf(stderr, "Failed! %d\n", res);
					break;
				}
				if(clkStatus.active)
					fprintf(stderr, "Period register(LE): %02X %02X, prescaler: %02X\n", clkStatus.periodRegL, clkStatus.periodRegH, clkStatus.prescaler);
				else
					fprintf(stderr, "Clock is off\n");
				break;
			case '2':
				fprintf(stderr, "Get power limit:\n");
				char buf2[21];
				LogsysPwrLimit limit={0};
				res=logsys_tx_get_pwr_limit(logsys, &limit);
				if(res<0){
					fprintf(stderr, "Failed! %d\n", res);
					break;
				}
				fprintf(stderr, "Max power: %d mA\n",
					limit==L450mA?450:limit==L700mA?700:limit==L950mA?950:-1
				);
				res=logsys_tx_get_pwr_corr(logsys, buf2);
				if(res<0){
					fprintf(stderr, "Failed! %d\n", res);
					break;
				}
				fprintf(stderr, "Got "); print_buf(buf2, 21); fprintf(stderr, "\n");
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
// 			case '?':
// 				fprintf(stderr, "JTAG scan\n");
// 				char jtag[2], num;
// 				res=logsys_tx_scan_jtag(logsys, &num, jtag);
// 				if(res<0)
// 					fprintf(stderr, "Failed! %d\n", res);
// 				fprintf(stderr, "Got %02X ", num); print_buf(jtag, 2); fprintf(stderr, "\n");
// 				break;
			case 'q':
				logsys_usb_close(logsys);
				logsys_usb_end();
				return 0;
		}
	}
}
