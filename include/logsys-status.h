#ifndef _LOGSYSDRV_STATUS_H
#define _LOGSYSDRV_STATUS_H

#include "logsys-common.h"

//supply voltage
double logsys_get_vcc_out(LogsysStatus status);
//JTAG reference voltage
double logsys_get_vjtag_out(LogsysStatus status);
//I/O reference voltage
double logsys_get_vio_out(LogsysStatus status);

//flowing current in milliamps
double logsys_get_current_ma(LogsysStatus status);

#endif //_LOGSYSDRV_STATUS_H
