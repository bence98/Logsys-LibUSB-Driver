#include <stdbool.h>
#include <stdlib.h>
#include "logsys-status.h"

double logsys_get_vcc_out(LogsysStatus status){
	short adcData=((status.vOutH<<8)&status.vOutL)&((1<<11)-1);
	return (2.56*3/1023)*adcData;
}

double logsys_get_vjtag_out(LogsysStatus status){
	short adcData=((status.vJtagRefH<<8)&status.vJtagRefL)&((1<<11)-1);
	return (2.56*5/1023)*adcData;
}

double logsys_get_vio_out(LogsysStatus status){
	short adcData=((status.vIoRefH<<8)&status.vIoRefL)&((1<<11)-1);
	return (2.56*5/1023)*adcData;
}
