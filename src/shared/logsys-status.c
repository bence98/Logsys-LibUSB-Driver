#include <stdbool.h>
#include <stdlib.h>
#include "logsys-status.h"

double logsys_get_vcc_out(LogsysStatus status){
	short adcData=((status.vOutH<<8)&status.vOutL)&((1<<11)-1);
	return (2.56*3/1023)*adcData;
}

double logsys_get_vjtag_out(LogsysStatus status){
	short adcData=((status.vJtagRefH<<8)|status.vJtagRefL)&((1<<11)-1);
	return (2.56*5/1023)*adcData;
}

double logsys_get_vio_out(LogsysStatus status){
	short adcData=((status.vIoRefH<<8)|status.vIoRefL)&((1<<11)-1);
	return (2.56*5/1023)*adcData;
}

double logsys_get_current_ma(LogsysStatus status){
	short adcData10=((status.iOutH<<14>>6)|status.iOutL),
		adcData200=((status.iOutDetailH<<14>>6)|status.iOutDetailL);
	return (2560/(512*200*.2))*adcData200+(2560/(512*10*.2))*adcData10;
}

bool logsys_is_vcc(LogsysStatus status){
	return status.flagPwr&1;
}

bool logsys_is_overcurr(LogsysStatus status){
	return status.flagPwr&2;
}

bool logsys_is_revcurr(LogsysStatus status){
	return status.flagPwr&4;
}

bool logsys_is_jtag_used(LogsysStatus status){
	return status.flagPin&1;
}

bool logsys_is_clk1_used(LogsysStatus status){
	return status.flagPin&2;
}

bool logsys_is_clk2_used(LogsysStatus status){
	return status.flagPin&4;
}

bool logsys_is_rst_used(LogsysStatus status){
	return status.flagPin&8;
}

bool logsys_is_sio_used(LogsysStatus status){
	return status.flagPin&16;
}

bool logsys_is_end_ni_used(LogsysStatus status){
	return status.flagPin&32;
}

bool logsys_is_end_int_used(LogsysStatus status){
	return status.flagPin&64;
}
