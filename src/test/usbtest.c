#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "logsys-usb.h"
#include "logsys-status.h"

void print_buf(char* buf, int len){
	for(int i=0;i<len;i++)
		printf("%02X", buf[i]);
}

int main(void){
	printf("Logsys USB driver test.\nExit with \"quit\" or CTRL+D\nDo not CTRL+C / SIGKILL!\n");
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
	
	char* cmd=NULL;
	size_t cmd_len=0;
	while(getline(&cmd, &cmd_len, stdin)!=-1){
		int res;
		if(strncmp(cmd, "status", sizeof("status")-1)==0){
				LogsysStatus status={0};
				res=logsys_tx_get_status(logsys, &status);
				if(res<0){
					fprintf(stderr, "Failed! %d\n", res);
					continue;
				}
				
				printf(" VCC | V_j | Vio |  I  |*+-|J12RSNI\n");
				printf("%5.2f|%5.2f|%5.2f|%5.2f|%c%c%c|%c%c%c%c%c%c%c\n",
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
		}else if(strncmp(cmd, "clk status", sizeof("clk status")-1)==0){
				LogsysClkStatus clkStatus={0};
				res=logsys_tx_clk_status(logsys, &clkStatus);
				if(res<0){
					fprintf(stderr, "Failed! %d\n", res);
					continue;
				}
				if(clkStatus.active)
					printf("Period register(LE): %02X %02X, prescaler: %02X\n", clkStatus.periodRegL, clkStatus.periodRegH, clkStatus.prescaler);
				else
					printf("Clock is off\n");
		}else if(strncmp(cmd, "pwrlim get", sizeof("pwrlim get")-1)==0){
				char buf2[21];
				LogsysPwrLimit limit={0};
				res=logsys_tx_get_pwr_limit(logsys, &limit);
				if(res<0){
					fprintf(stderr, "Failed! %d\n", res);
					continue;
				}
				printf("Max power: %d mA\n",
					limit==L450mA?450:limit==L700mA?700:limit==L950mA?950:-1
				);
				res=logsys_tx_get_pwr_corr(logsys, buf2);
				if(res<0){
					fprintf(stderr, "Failed! %d\n", res);
					continue;
				}
				printf("Got "); print_buf(buf2, 21); printf("\n");
		}else if(strncmp(cmd, "vcc on", sizeof("vcc on")-1)==0){
				res=logsys_tx_set_vcc(logsys, true);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
		}else if(strncmp(cmd, "vcc off", sizeof("vcc off")-1)==0){
				res=logsys_tx_set_vcc(logsys, false);
				if(res<0)
					fprintf(stderr, "Failed! %d\n", res);
		}else if(strncmp(cmd, "jtag scan", sizeof("jtag scan")-1)==0){
				bool ready;
				res=logsys_tx_jtag_begin(logsys, MODE_ECHO, &ready);
				if(res<0){
					fprintf(stderr, "Begin failed! %d\n", res);
					continue;
				}
				if(!ready){
					fprintf(stderr, "JTAG unavailable\n");
					continue;
				}
				uint32_t devs[16];
				int dev_len;
				res=logsys_jtag_scan(logsys, devs, 16, &dev_len);
				if(res<0){
					fprintf(stderr, "Scan failed! %d\n", res);
					continue;
				}
				printf("%d devices:\n", dev_len);
				for(int i=0;i<dev_len;i++)
					printf("  0x%.8X\n", devs[i]);
				res=logsys_tx_jtag_end(logsys);
				if(res<0)
					fprintf(stderr, "End failed! %d\n", res);
		}else if(strncmp(cmd, "conf svf", sizeof("conf svf")-1)==0){
			FILE* f=fopen("/home/bence/Dokumentumok/bme/digit1/impact/sz_geektime.svf", "r");
			logsys_jtag_dl_svf(logsys, f);
			fclose(f);
		}else if(strncmp(cmd, "quit", sizeof("quit")-1)==0){
			break;
		}else{
			fprintf(stderr, "Unknown command! %s\n", cmd);
		}
	}
	printf("Exiting...\n");
	if(cmd!=NULL) free(cmd);
	logsys_usb_close(logsys);
	logsys_usb_end();
	return 0;
}
