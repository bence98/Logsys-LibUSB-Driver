/** @file usb.h LOGSYS USB functions */
#ifndef _LOGSYSDRV_USB_H
#define _LOGSYSDRV_USB_H

#include "logsys/common.h"

/** Set up LibUSB
  * @note You should always call it, at the start of the program, before calling any other LOGSYS functions
  */
bool logsys_usb_init();
/** Release LibUSB
  * @note You should call this right before your program quits, to ensure a clean exit
  */
bool logsys_usb_end();
/** Open the USB device (LOGSYS)
  * @note pDev may be NULL, in which case we try to find a device
  * @returns a LibUSB handle, or NULL on error
  */
libusb_device_handle* logsys_usb_open(libusb_device* pDev);
/** Release and close the LOGSYS device */
void logsys_usb_close(libusb_device_handle* dev);
/** Register a hotplug listener for LOGSYS devices
  * @param evt_type Type of event: connect/disconnect
  * @param callback The handler to call on a hotplug event
  * @param hndl The event handle
  * @warning Don't forget to call `libusb_hotplug_deregister_callback()`!
  * @note You will need to periodically call `libusb_handle_events_completed()` in the main thread
  */
bool logsys_hotplug_enable(libusb_hotplug_event evt_type, libusb_hotplug_callback_fn callback, /*out*/libusb_hotplug_callback_handle* hndl);

/** Get the @link LogsysStatus status@endlink of the device */
int logsys_tx_get_status(libusb_device_handle* dev, /*out*/LogsysStatus* data);

/** Get the @link LogsysClkStatus clock status@endlink of the device */
int logsys_tx_clk_status(libusb_device_handle* dev, /*out*/LogsysClkStatus* data);
/** Starts the clock */
int logsys_clk_start(libusb_device_handle* dev, int freqHz, bool* success);
/** Stops the clock */
int logsys_clk_stop(libusb_device_handle* dev, bool* was_running);

/** Sets RESET pin */
int logsys_tx_set_reset(libusb_device_handle* dev, bool reset, /*out*/bool* success);
/** Gets RESET pin */
int logsys_tx_get_reset(libusb_device_handle* dev, /*out*/bool* reset);

/** Gets the power limit */
int logsys_tx_get_pwr_limit(libusb_device_handle* dev, /*out*/LogsysPwrLimit* limit);
/** Sets the power limit */
int logsys_tx_set_pwr_limit(libusb_device_handle* dev, LogsysPwrLimit limit);
/** Correction coefficients
  * @warning TODO parsing unimplemented!
  * @param dev LOGSYS device
  * @param ch -> char[21]
  */
int logsys_tx_get_pwr_corr(libusb_device_handle* dev, /*out*/char* ch);

/** Sets VCC pin */
int logsys_tx_set_vcc(libusb_device_handle* dev, bool vcc);
/** Gets VCC status */
int logsys_tx_get_vcc(libusb_device_handle* dev, /*out*/ bool* vcc);

/** Gets reverse current tolerance. Values in milliamps */
int logsys_tx_get_rev_curr(libusb_device_handle* dev, double* mAmps);
/** Sets reverse current tolerance. Values in milliamps */
int logsys_tx_set_rev_curr(libusb_device_handle* dev, double mAmps);

/** Gets the active (non-interrupting) function */
int logsys_tx_get_active_func(libusb_device_handle* dev, /*out*/LogsysFunction* func);

/** Begin JTAG transmission */
int logsys_tx_jtag_begin(libusb_device_handle* dev, LogsysJtagMode mode, /*out*/bool* ready);
/** End JTAG transmission */
int logsys_tx_jtag_end(libusb_device_handle* dev);

/** Begin BitBang Serial I/O */
int logsys_tx_serial_begin(libusb_device_handle* dev, bool* success);
/** Change BitBang Serial I/O clock frequency */
int logsys_tx_serial_change_clk(libusb_device_handle* dev, int freqHz);
/** End BitBang Serial I/O */
int logsys_tx_serial_end(libusb_device_handle* dev);

/** Performs a JTAG boundary-scan & returns the device IDs in the `jtag_devs` array and saves the number of devices in `found_devs`
  * NOTE: if there's more than `max_devs`, then only the first `max_devs` gets returned, and `found_devs` will be set to `max_devs`
  */
int logsys_jtag_scan(libusb_device_handle* dev, /*out*/uint32_t jtag_devs[], int max_devs, /*out*/int* found_devs);
/** Gets current JTAG mode */
int logsys_jtag_get_mode(libusb_device_handle* dev, /*out*/LogsysJtagMode* mode);
/** Selects between @link MODE_CMP Write & Compare mode@endlink and @link MODE_ECHO Write & Echo@endlink modes */
int logsys_jtag_set_mode(libusb_device_handle* dev, LogsysJtagMode mode);
/** In JTAG @link MODE_CMP Write & Compare mode@endlink, this query reads the check results */
int logsys_jtag_check_error(libusb_device_handle* dev, /*out*/bool* error);

/** Begin USART */
int logsys_tx_usart_begin(libusb_device_handle* dev, unsigned int baud, bool usrt, /*out*/bool* success);
/** End USART */
int logsys_tx_usart_end(libusb_device_handle* dev);

#endif //_LOGSYSDRV_USB_H
