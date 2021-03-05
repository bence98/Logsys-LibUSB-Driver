/** @file control.h Basic LOGSYS controls (VCC, CLK, RST etc.) */
#ifndef _LOGSYSDRV_CTRL_H
#define _LOGSYSDRV_CTRL_H

#include "logsys/common.h"

/** Get the @link LogsysStatus status@endlink of the device */
int logsys_get_status(libusb_device_handle* dev, /*out*/LogsysStatus* data);

/** Get the @link LogsysClkStatus clock status@endlink of the device */
int logsys_clk_status(libusb_device_handle* dev, /*out*/LogsysClkStatus* data);
/** Starts the clock */
int logsys_clk_start(libusb_device_handle* dev, int freqHz, /*out*/bool* success);
/** Stops the clock */
int logsys_clk_stop(libusb_device_handle* dev, bool* was_running);

/** Sets RESET pin */
int logsys_set_reset(libusb_device_handle* dev, bool reset, /*out*/bool* success);
/** Gets RESET pin */
int logsys_get_reset(libusb_device_handle* dev, /*out*/bool* reset);

/** Gets the power limit */
int logsys_get_pwr_limit(libusb_device_handle* dev, /*out*/LogsysPwrLimit* limit);
/** Sets the power limit */
int logsys_set_pwr_limit(libusb_device_handle* dev, LogsysPwrLimit limit);
/** Correction coefficients
  * @warning TODO parsing unimplemented!
  * @param dev LOGSYS device
  * @param ch -> char[21]
  */
int logsys_get_pwr_corr(libusb_device_handle* dev, /*out*/char* ch);

/** Sets VCC pin */
int logsys_set_vcc(libusb_device_handle* dev, bool vcc);
/** Gets VCC status */
int logsys_get_vcc(libusb_device_handle* dev, /*out*/bool* vcc);

/** Gets reverse current tolerance. Values in milliamps */
int logsys_get_rev_curr(libusb_device_handle* dev, double* mAmps);
/** Sets reverse current tolerance. Values in milliamps */
int logsys_set_rev_curr(libusb_device_handle* dev, double mAmps);

/** Gets the active (non-interrupting) function */
int logsys_get_active_func(libusb_device_handle* dev, /*out*/LogsysFunction* func);

/** Gets the firmware version */
int logsys_fw_get_ver(libusb_device_handle* dev, /*out*/LogsysFirmwareVersion* func);

/** Enter DFU mode */
int logsys_fw_update(libusb_device_handle* dev);

#endif //_LOGSYSDRV_CTRL_H
