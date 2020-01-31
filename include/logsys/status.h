/** @file status.h LOGSYS status code processing */
#ifndef _LOGSYSDRV_STATUS_H
#define _LOGSYSDRV_STATUS_H

#include "logsys/common.h"

/** @returns Supply voltage */
double logsys_get_vsupply_out(LogsysStatus status);
/** @returns JTAG reference voltage */
double logsys_get_vjtag_out(LogsysStatus status);
/** @returns I/O reference voltage */
double logsys_get_vio_out(LogsysStatus status);

/** @returns flowing current in milliamps */
double logsys_get_current_ma(LogsysStatus status);

/** @returns clock frequency in Hz
  * @param status The clock status object
  * @param divisor 2 for the standard clock and 4 for the Serial port clock
  */
double logsys_get_clk_freq_hz(LogsysClkStatus status, double divisor);

/** @returns VCC on? */
bool logsys_is_vcc(LogsysStatus status);
/** @returns Overcurrent shutdown? */
bool logsys_is_overcurr(LogsysStatus status);
/** @returns Reverse current shutdown? */
bool logsys_is_revcurr(LogsysStatus status);

/** @returns JTAG interface in use? */
bool logsys_is_jtag_used(LogsysStatus status);
/** @returns CLOCK1 in use? */
bool logsys_is_clk1_used(LogsysStatus status);
/** @returns CLOCK2 in use? */
bool logsys_is_clk2_used(LogsysStatus status);
/** @returns RESET pin in use? */
bool logsys_is_rst_used(LogsysStatus status);
/** @returns Serial I/O pins in use? */
bool logsys_is_sio_used(LogsysStatus status);
/** @returns EP1/EP2 (non-interrupting) in use? */
bool logsys_is_end_ni_used(LogsysStatus status);
/** @returns EP3/EP4 (interrupting) in use? */
bool logsys_is_end_int_used(LogsysStatus status);

#endif //_LOGSYSDRV_STATUS_H
