#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libxsvf.h"
#include "logsys/usb.private.h"
#include "logsys/jconf.h"

struct jtag_lines{
	//0: low, 1: high, -1: keep/don't care
	char tms, tdi, tdo, mask;
};

struct udata_s{
	libusb_device_handle* dev;
	FILE* svfFile;
	struct jtag_lines out_buf[4096];
	short o_ptr;
	bool cmpMode, wasCmpErr;
};

void lsvf_write_pack(libusb_device_handle* dev, char bulk_out[1024][4], short bulk_len, bool* wasCmpErr){
	char raw[4096];
	for(int i=0;i<bulk_len;i++){
		memcpy(raw+4*i, bulk_out[i], 4);
	}
	libusb_bulk_transfer(dev, LOGSYS_OUT_EP1, raw, 4*bulk_len, NULL, 0);
	logsys_jtag_check_error(dev, wasCmpErr);
}

void lsvf_flush_iobuf(struct udata_s* u){
	char bulk_out[1024][4]={0};
	short bulk_len=0;
	for(int i=0;i<u->o_ptr;i++){
		struct jtag_lines ln=u->out_buf[i];
		
		if(ln.tdi==-1){
			//TMS write
			char count=0, data=0, dbit=1, chk=0, cmask=0;
			while(ln.tdi==-1&&ln.tms!=-1){
				i++;
				data|=ln.tms*dbit;
				if(ln.tdo!=-1){
					cmask|=dbit;
					chk|=ln.tdo*dbit;
				}
				dbit<<=1;
				count++;
				if(count==8) break;
				if(i<u->o_ptr) ln=u->out_buf[i];
				else break;
			}
			i--;
			bulk_out[bulk_len][0]=0x80|(count-1);
			bulk_out[bulk_len][1]=data;
			bulk_out[bulk_len][2]=chk;
			bulk_out[bulk_len][3]=cmask;
			bulk_len++;
		}else{
			//TDI write
			char count=0, data=0, dbit=1, chk=0, cmask=0;
			bool tmsAfter=false;
			while(ln.tdi!=-1&&!tmsAfter){
				i++;
				data|=ln.tdi*dbit;
				if(ln.tdo!=-1){
					cmask|=dbit;
					chk|=ln.tdo*dbit;
				}
				dbit<<=1;
				count++;
				if(ln.tms) tmsAfter=true;
				if(count==8) break;
				if(i<u->o_ptr) ln=u->out_buf[i];
				else break;
			}
			i--;
			bulk_out[bulk_len][0]=(0x40*tmsAfter)|(count-1);
			bulk_out[bulk_len][1]=data;
			bulk_out[bulk_len][2]=chk;
			bulk_out[bulk_len][3]=cmask;
			bulk_len++;
		}
		
		if(bulk_len==1024){
			lsvf_write_pack(u->dev, bulk_out, bulk_len, &u->wasCmpErr);
			bulk_len=0;
		}
	}
	if(bulk_len>0){
		lsvf_write_pack(u->dev, bulk_out, bulk_len, &u->wasCmpErr);
	}
	u->o_ptr=0;
}

int lsvf_host_setup(struct libxsvf_host *h){
	bool ok;
	struct udata_s* u=h->user_data;
	logsys_jtag_begin(u->dev, MODE_CMP, &ok);
	return ok?0:-1;
}

int lsvf_host_shutdown(struct libxsvf_host *h){
	struct udata_s* u=h->user_data;
	lsvf_flush_iobuf(u);
	logsys_jtag_end(u->dev);
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
	struct udata_s* u=h->user_data;
	return fgetc(u->svfFile);
}

int lsvf_host_pulse_tck(struct libxsvf_host *h, int tms, int tdi, int tdo, int rmask, int sync){
	struct udata_s* u=h->user_data;
	u->out_buf[u->o_ptr].tms=tms;
	u->out_buf[u->o_ptr].tdi=tdi;
	u->out_buf[u->o_ptr].tdo=tdo;
	u->out_buf[u->o_ptr].mask=rmask;
	u->o_ptr++;
	if(sync||u->o_ptr>=4096) lsvf_flush_iobuf(u);
	return u->wasCmpErr?-1:1;
}

void lsvf_host_pulse_sck(struct libxsvf_host *h){}

void lsvf_host_set_trst(struct libxsvf_host *h, int v){}

int lsvf_host_set_frequency(struct libxsvf_host *h, int v){
	return 0;
}

void lsvf_host_report_tapstate(struct libxsvf_host *h){}

void lsvf_host_report_device(struct libxsvf_host *h, unsigned long idcode){}

void lsvf_host_report_status(struct libxsvf_host *h, const char *message){}

void lsvf_host_report_error(struct libxsvf_host *h, const char *file, int line, const char *message){
	fprintf(stderr, "[%s:%d] %s\n", file, line, message);
}

static void *lsvf_host_realloc(struct libxsvf_host *h, void *ptr, int size, enum libxsvf_mem which)
{
	return realloc(ptr, size);
}

int lsvf_play(libusb_device_handle* dev, void* f, bool svfMode){
	struct udata_s u={
		dev,
		f,
		0
	};
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
		.user_data = &u
	};
	return libxsvf_play(&h, svfMode?LIBXSVF_MODE_SVF:LIBXSVF_MODE_XSVF);
}

int logsys_jtag_dl_svf(libusb_device_handle* dev, void* f){
	return lsvf_play(dev, f, true);
}

int logsys_jtag_dl_xsvf(libusb_device_handle* dev, void* f){
	return lsvf_play(dev, f, false);
}

void* logsys_conv_bit2svf(char* bitfile){
	int fdLastDir=open(".", O_DIRECTORY);
	chdir("/tmp");
	void* impactCmd=popen("$XILINX/bin/lin64/impact -batch", "w");
	fprintf(impactCmd, "setPreference -pref StartupClock:AUTO_CORRECTION\n\
setPreference -pref AutoSignature:FALSE\nsetPreference -pref KeepSVF:FALSE\n\
setPreference -pref ConcurrentMode:FALSE\nsetPreference -pref UseHighz:FALSE\n\
setPreference -pref ConfigOnFailure:STOP\nsetPreference -pref UserLevel:NOVICE\n\
setPreference -pref MessageLevel:DETAILED\nsetPreference -pref svfUseTime:TRUE\n\
setMode -bs\nsetMode -bs\nsetCable -port svf -file \"bit2svf.svf\"\n\
addDevice -p 1 -file \"%s\"\n\nProgram -p 1 -defaultVersion 0\nquit\n", bitfile);
	fflush(impactCmd);
	int res=pclose(impactCmd);
	fchdir(fdLastDir);
	if(WIFEXITED(res)&&WEXITSTATUS(res)==0){
		return fopen("/tmp/bit2svf.svf", "r");
	}else{
		return NULL;
	}
}
