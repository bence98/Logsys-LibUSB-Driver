#include <stdbool.h>
#include <stdlib.h>
#include "logsys-status.h"

double logsys_get_vcc_out(LogsysStatus status){
	short adcData=TO_WORD(status.vOutH, status.vOutL)&((1<<11)-1);
	return (2.56*3/1023)*adcData;
}

double logsys_get_vjtag_out(LogsysStatus status){
	short adcData=TO_WORD(status.vJtagRefH, status.vJtagRefL)&((1<<11)-1);
	return (2.56*5/1023)*adcData;
}

double logsys_get_vio_out(LogsysStatus status){
	short adcData=TO_WORD(status.vIoRefH, status.vIoRefL)&((1<<11)-1);
	return (2.56*5/1023)*adcData;
}

double logsys_get_current_ma(LogsysStatus status){
	short adcData10=((status.iOutH<<14>>6)|status.iOutL),
		adcData200=((status.iOutDetailH<<14>>6)|status.iOutDetailL);
	return (2560/(512*200*.2))*adcData200+(2560/(512*10*.2))*adcData10;
}

double logsys_get_clk_freq_hz(LogsysClkStatus status, double divisor){
	static double mcuFreqHz=16000000; //16 MHz
	static double prescaler[]={1.0, 1.0/8, 1.0/64, 1.0/256, 1.0/1024};
	return mcuFreqHz*prescaler[status.prescaler]/(TO_WORD(status.periodRegH, status.periodRegL)*divisor);
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

LogsysClkStatus logsys_create_clk_status(double freqHz, double divisor){
	LogsysClkStatus status={0};
	static double mcuFreqHz=16000000; //16 MHz
	static double prescaler[]={1.0, 1.0/8, 1.0/64, 1.0/256, 1.0/1024};
	
	double error=0, minError=freqHz, newFreqHz;
	short prescalerVal, perRegVal;
	
	for(int i=0;i<5;i++){
		int tryPerRegVal=(mcuFreqHz*prescaler[i]/(2*freqHz));
		if(tryPerRegVal<0||tryPerRegVal>65535) continue;
		newFreqHz=mcuFreqHz*prescaler[i]/(tryPerRegVal*2);
		error=abs(freqHz-newFreqHz);
		if(error<minError){
			minError=error;
			prescalerVal=i+1;
			perRegVal=tryPerRegVal-1;
		}
		if(error==0) break;
	}
	
	status.periodRegL=perRegVal & 0xff;
	status.periodRegH=(perRegVal & 0xff00) >> 8;
	status.prescaler=prescalerVal;
	return status;
}
