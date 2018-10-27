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

//clock frequency in Hz
//`divisor` is 2 for the standard clock and 4 for the Serial port clock
double logsys_get_clk_freq_hz(LogsysClkStatus status, double divisor);

//VCC on?
bool logsys_is_vcc(LogsysStatus status);
//Overcurrent shutdown?
bool logsys_is_overcurr(LogsysStatus status);
//Reverse current shutdown?
bool logsys_is_revcurr(LogsysStatus status);

//JTAG interface in use?
bool logsys_is_jtag_used(LogsysStatus status);
//CLOCK1 pin in use?
bool logsys_is_clk1_used(LogsysStatus status);
//CLOCK2 pin in use?
bool logsys_is_clk2_used(LogsysStatus status);
//RESET pin in use?
bool logsys_is_rst_used(LogsysStatus status);
//Serial I/O pins in use?
bool logsys_is_sio_used(LogsysStatus status);
//EP1/EP2 (non-interrupting) in use?
bool logsys_is_end_ni_used(LogsysStatus status);
//EP3/EP4 (interrupting) in use?
bool logsys_is_end_int_used(LogsysStatus status);

#endif //_LOGSYSDRV_STATUS_H
