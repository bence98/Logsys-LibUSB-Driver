#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "libxsvf.h"
#include "logsys-usb.h"

struct udata{
	libusb_device_handle* dev;
	FILE* svfFile;
	char out_buf[4096];
	short o_ptr;
	char chk_buf[4096];
	char chk_mask[4096];
	bool tms, chk;
};

void lsvf_flush_iobuf(struct udata* u){
	short bytes=(u->o_ptr+7)/8;
	char paket[4*bytes];
	
	//i: byte index in out_buf
	for(int i=0;i<bytes;i++){
		int paket_off=u->chk?4*i:2*i;
		char bits_remain=u->o_ptr-8*i;
		if(bits_remain>8) bits_remain=8;
		
		if(u->tms) paket[paket_off]=0x40;
		paket[paket_off]|=bits_remain;
		for(char x=0;x<bits_remain;x++){
			paket[paket_off+1]=(paket[paket_off+1]<<1)|(u->out_buf[i]&1);
			u->out_buf[i]>>1;
			if(u->chk){
				paket[paket_off+2]=(paket[paket_off+2]<<1)|(u->chk_buf[i]&1);
				u->chk_buf[i]>>1;
				paket[paket_off+3]=(paket[paket_off+3]<<1)|(u->chk_mask[i]&1);
				u->chk_mask[i]>>1;
			}
		}
	}
	
	if(u->chk){
		libusb_bulk_transfer(u->dev, LOGSYS_OUT_EP1, paket, 4*bytes, NULL, 0);
		bool err;
		logsys_jtag_check_error(u->dev, &err);
		if(err){
			fprintf(stderr, "[FATAL] Compare error!\n");
			exit(1);
		}
	}else{
		libusb_bulk_transfer(u->dev, LOGSYS_OUT_EP1, paket, 2*bytes, NULL, 0);
		libusb_bulk_transfer(u->dev, LOGSYS_IN_EP2,  paket, 2*bytes, NULL, 0);
	}
}

void lsvf_toggmode(struct udata* u){
	lsvf_flush_iobuf(u);
	logsys_jtag_set_mode(u->dev, u->chk?MODE_CMP:MODE_ECHO);
	u->chk=!(u->chk);
}

void lsvf_tms(struct udata* u, int tms){
	if(tms!=u->tms) lsvf_flush_iobuf(u);
	u->tms=tms;
}

int lsvf_host_setup(struct libxsvf_host *h){
	bool ok;
	struct udata* u=h->user_data;
	logsys_tx_jtag_begin(u->dev, u->chk?MODE_CMP:MODE_ECHO, &ok);
	return ok?0:-1;
}

int lsvf_host_shutdown(struct libxsvf_host *h){
	struct udata* u=h->user_data;
	logsys_tx_jtag_end(u->dev);
	return 0;
}

int lsvf_host_pulse_tck(struct libxsvf_host *h, int tms, int tdi, int tdo, int rmask, int sync);

void lsvf_host_udelay(struct libxsvf_host *h, long usecs, int tms, long num_tck){
	if (num_tck > 0) {
		struct timeval tv1, tv2;
		gettimeofday(&tv1, NULL);
		while (num_tck > 0) {
			lsvf_host_pulse_tck(h, tms, -1, -1, 0, 0);
			num_tck--;
		}
		gettimeofday(&tv2, NULL);
		if (tv2.tv_sec > tv1.tv_sec) {
			usecs -= (1000000 - tv1.tv_usec) + (tv2.tv_sec - tv1.tv_sec - 1) * 1000000;
			tv1.tv_usec = 0;
		}
		usecs -= tv2.tv_usec - tv1.tv_usec;
	}
	if (usecs > 0) {
		usleep(usecs);
	}
}

int lsvf_host_getbyte(struct libxsvf_host *h){
	struct udata* u=h->user_data;
	return fgetc(u->svfFile);
}

int lsvf_host_pulse_tck(struct libxsvf_host *h, int tms, int tdi, int tdo, int rmask, int sync){
	struct udata* u=h->user_data;
	lsvf_tms(u, tms);
	short o_idx=(u->o_ptr)/8;
	if((u->chk)!=(tdo!=-1)) lsvf_toggmode(u);
	if(tdi==-1){
		//keep last
		tdi=u->out_buf[o_idx]&1;
	}
	u->out_buf[o_idx]=(u->out_buf[o_idx]<<1)|tdi;
	if(u->chk){
		u->chk_buf[o_idx]<<=1;
		u->chk_mask[o_idx]<<=1;
		if(tdo>=0){
			u->chk_buf[o_idx]|tdo;
			u->chk_mask[o_idx]|1;
		}
	}
	u->o_ptr++;
	if(u->o_ptr>=4096) lsvf_flush_iobuf(u);
	return tdo<0?0:tdo;
}

void lsvf_host_pulse_sck(struct libxsvf_host *h){}

void lsvf_host_set_trst(struct libxsvf_host *h, int v){}

int lsvf_host_set_frequency(struct libxsvf_host *h, int v){}

void lsvf_host_report_tapstate(struct libxsvf_host *h){}

void lsvf_host_report_device(struct libxsvf_host *h, unsigned long idcode){}

void lsvf_host_report_status(struct libxsvf_host *h, const char *message){}

void lsvf_host_report_error(struct libxsvf_host *h, const char *file, int line, const char *message){}

static int realloc_maxsize[LIBXSVF_MEM_NUM];

static void *lsvf_host_realloc(struct libxsvf_host *h, void *ptr, int size, enum libxsvf_mem which)
{
	struct udata_s *u = h->user_data;
	if (size > realloc_maxsize[which])
		realloc_maxsize[which] = size;
	
	return realloc(ptr, size);
}

int logsys_jtag_dl_svf(libusb_device_handle* dev, void* f){
	struct libxsvf_host h={
		.udelay = lsvf_host_udelay,
		.setup = lsvf_host_setup,
		.shutdown = lsvf_host_shutdown,
		.getbyte = lsvf_host_getbyte,
		.pulse_tck = lsvf_host_pulse_tck,
		.pulse_sck = lsvf_host_pulse_sck,
		.set_trst = lsvf_host_set_trst,
		.set_frequency = lsvf_host_set_frequency,
		.report_tapstate = lsvf_host_report_tapstate,
		.report_device = lsvf_host_report_device,
		.report_status = lsvf_host_report_status,
		.report_error = lsvf_host_report_error,
		.realloc = lsvf_host_realloc,
		.user_data = malloc(sizeof(struct udata))
	};
	((struct udata*)h.user_data)->dev=dev;
	((struct udata*)h.user_data)->svfFile=f;
	return libxsvf_play(&h, LIBXSVF_MODE_SVF);
}
