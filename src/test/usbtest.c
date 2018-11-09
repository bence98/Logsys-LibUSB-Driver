#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "logsys/usb.h"
#include "logsys/status.h"
#include "logsys/jconf.h"

void print_buf(char* buf, int len){
	for(int i=0;i<len;i++)
		printf("%02X", buf[i]);
}

size_t parse_tokens(char* tokens[], size_t token_count){
	char* ln=NULL;
	size_t ln_len=0;
	if(getline(&ln, &ln_len, stdin)==-1){
		free(ln);
		return 0;
	}
	
	for(size_t idx=0;idx<token_count;idx++){
		if(tokens[idx]!=NULL)
			free(tokens[idx]);
		tokens[idx]=malloc(ln_len+1);
	}
	
	size_t tok_len=0;
	int token=0;
	char quot_char=0;
	tokens[token]=malloc(ln_len);
	for(size_t idx=0;idx<ln_len;idx++){
		if(ln[idx]==' '&&quot_char==0 || ln[idx]==quot_char){
			if(tok_len>0){
				tokens[token][tok_len]='\0';
				token++;
				tok_len=0;
				quot_char=0;
			}
		}else if((ln[idx]=='\''||ln[idx]=='"')&&quot_char==0)
			quot_char=ln[idx];
		else if(ln[idx]=='\r'||ln[idx]=='\n'){
			tokens[token][tok_len]='\0';
			token++;
			free(ln);
			return token;
		}
		else{
			tokens[token][tok_len]=ln[idx];
			tok_len++;
		}
	}
	
	free(ln);
	return token_count;
}

bool cmd_cmp(char* tokens[], size_t idx, char* to_find){
	return strncmp(tokens[idx], to_find, strlen(to_find))==0;
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
	
	char* cmd[3]={0};
	while(parse_tokens(cmd, 3)>0){
		int res;
		if(cmd_cmp(cmd, 0, "status")){
				LogsysStatus status={0};
				res=logsys_tx_get_status(logsys, &status);
				if(res<0){
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
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
		}else if(cmd_cmp(cmd, 0, "clk")){
			if(cmd_cmp(cmd, 1, "status")){
				LogsysClkStatus clkStatus={0};
				res=logsys_tx_clk_status(logsys, &clkStatus);
				if(res<0){
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
					continue;
				}
				if(clkStatus.active)
					printf("Period register(LE): %02X %02X, prescaler: %02X\n", (unsigned char)clkStatus.periodRegL, (unsigned char)clkStatus.periodRegH, (unsigned char)clkStatus.prescaler);
				else
					printf("Clock is off\n");
			}else if(cmd_cmp(cmd, 1, "start")){
				bool ok;
				res=logsys_clk_start(logsys, atoi(cmd[2]), &ok);
			}else if(cmd_cmp(cmd, 1, "stop")){
				bool ok;
				res=logsys_clk_stop(logsys, &ok);
				if(res<0){
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
					continue;
				}
				if(ok)
					printf("Stopped clock\n");
				else
					fprintf(stderr, "Clock was off!\n");
			}else{
				fprintf(stderr, "Invalid operation: 'clk %s'. Allowed: 'status', 'start', 'stop'\n", cmd[1]);
			}
		}else if(cmd_cmp(cmd, 0, "pwrlim")){
			if(cmd_cmp(cmd, 1, "get")){
				char buf2[21];
				LogsysPwrLimit limit={0};
				res=logsys_tx_get_pwr_limit(logsys, &limit);
				if(res<0){
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
					continue;
				}
				printf("Max power: %d mA\n",
					limit==L450mA?450:limit==L700mA?700:limit==L950mA?950:-1
				);
				res=logsys_tx_get_pwr_corr(logsys, buf2);
				if(res<0){
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
					continue;
				}
				printf("Got "); print_buf(buf2, 21); printf("\n");
			}else{
				fprintf(stderr, "Invalid operation: 'pwrlim %s'. Allowed: 'get'\n", cmd[1]);
			}
		}else if(cmd_cmp(cmd, 0, "vcc")){
			if(cmd_cmp(cmd, 1, "on")){
				res=logsys_tx_set_vcc(logsys, true);
				if(res<0)
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
			}else if(cmd_cmp(cmd, 1, "off")){
				res=logsys_tx_set_vcc(logsys, false);
				if(res<0)
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
			}else if(cmd_cmp(cmd, 1, "get")){
				bool vcc;
				res=logsys_tx_get_vcc(logsys, &vcc);
				if(res<0)
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
				printf("VCC %s\n", vcc?"high":"low");
			}else{
				fprintf(stderr, "Invalid operation: 'vcc %s'. Allowed: 'on', 'off', 'get'\n", cmd[1]);
			}
		}else if(cmd_cmp(cmd, 0, "rst")){
			if(cmd_cmp(cmd, 1, "on")){
				bool ok;
				res=logsys_tx_set_reset(logsys, true, &ok);
				if(res<0)
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
				if(!ok)
					fprintf(stderr, "Could not drive RST! (already in use)\n");
			}else if(cmd_cmp(cmd, 1, "off")){
				bool ok;
				res=logsys_tx_set_reset(logsys, false, &ok);
				if(res<0)
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
				if(!ok)
					fprintf(stderr, "Could not drive RST! (already in use?)\n");
			}else if(cmd_cmp(cmd, 1, "get")){
				bool rst;
				res=logsys_tx_get_reset(logsys, &rst);
				if(res<0)
					fprintf(stderr, "Failed! %s\n", libusb_error_name(res));
				printf("RST %s\n", rst?"high":"low");
			}else{
				fprintf(stderr, "Invalid operation: 'rst %s'. Allowed: 'on', 'off', 'get'\n", cmd[1]);
			}
		}else if(cmd_cmp(cmd, 0, "jtag")){
			if(cmd_cmp(cmd, 1, "scan")){
				bool ready;
				res=logsys_tx_jtag_begin(logsys, MODE_ECHO, &ready);
				if(res<0){
					fprintf(stderr, "Begin failed! %s\n", libusb_error_name(res));
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
					fprintf(stderr, "Scan failed! %s\n", libusb_error_name(res));
					continue;
				}
				printf("%d devices:\n", dev_len);
				for(int i=0;i<dev_len;i++)
					printf("  0x%.8X\n", devs[i]);
				res=logsys_tx_jtag_end(logsys);
				if(res<0)
					fprintf(stderr, "End failed! %s\n", libusb_error_name(res));
			}else{
				fprintf(stderr, "Invalid operation: 'jtag %s'. Did you mean 'conf'?\n", cmd[1]);
			}
		}else if(cmd_cmp(cmd, 0, "conf")){
			if(cmd_cmp(cmd, 1, "svf")){
				FILE* f=fopen(cmd[2], "r");
				if(f==NULL){
					fprintf(stderr, "Could not open XSVF file!\n");
					continue;
				}
				res=logsys_jtag_dl_svf(logsys, f);
				fclose(f);
				printf("Configuration finished (%d)\n", res);
			}else if(cmd_cmp(cmd, 1, "xsvf")){
				FILE* f=fopen(cmd[2], "r");
				if(f==NULL){
					fprintf(stderr, "Could not open XSVF file!\n");
					continue;
				}
				res=logsys_jtag_dl_xsvf(logsys, f);
				fclose(f);
				printf("Configuration finished (%d)\n", res);
			}else if(cmd_cmp(cmd, 1, "bit")){
				FILE* f=logsys_conv_bit2svf(cmd[2]);
				if(f==NULL){
					fprintf(stderr, "Could not convert BIT file! (missing XILINX?)\n");
					continue;
				}
				res=logsys_jtag_dl_svf(logsys, f);
				fclose(f);
				printf("Configuration finished (%d)\n", res);
			}else{
				fprintf(stderr, "Invalid operation: 'conf %s' (unsupported format?)\n", cmd[1]);
			}
		}else if(cmd_cmp(cmd, 0, "quit")){
			break;
		}else{
			fprintf(stderr, "Unknown command! %s\n", cmd[0]);
		}
	}
	printf("Exiting...\n");
	logsys_usb_close(logsys);
	logsys_usb_end();
	return 0;
}
