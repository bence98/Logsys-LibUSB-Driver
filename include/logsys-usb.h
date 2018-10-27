#ifndef _LOGSYSDRV_USB_H
#define _LOGSYSDRV_USB_H

#include "logsys-common.h"

//interface number
#define LOGSYS_IFN_DEV 0
#define LOGSYS_IFN_COM 1
//endpoints: - interface0
//non-interrupting function endpoints
//see also: LogsysFunction
#define LOGSYS_OUT_EP1 0x01
#define LOGSYS_IN_EP2  0x82
//interrupting function endpoints
#define LOGSYS_OUT_EP3 0x03
#define LOGSYS_IN_EP4  0x84
// - interface1
//COM endpoints
#define LOGSYS_OUT_EP5 0x05
#define LOGSYS_IN_EP6  0x86

#define LOGSYS_REQTYP_IN  0xc0
#define LOGSYS_REQTYP_OUT 0x40

#define LOGSYS_DLEN_REQ48 24

bool logsys_usb_init();
bool logsys_usb_end();
libusb_device_handle* logsys_usb_open(libusb_device* pDev);
void logsys_usb_close(libusb_device_handle* dev);
//don't forget to `libusb_hotplug_deregister_callback()`!
/*async*/ bool logsys_hotplug_enable(libusb_hotplug_event evt_type, libusb_hotplug_callback_fn callback, /*out*/libusb_hotplug_callback_handle* hndl);

int logsys_tx_get_status(libusb_device_handle* dev, /*out*/LogsysStatus* data);

int logsys_tx_clk_status(libusb_device_handle* dev, /*out*/LogsysClkStatus* data);
int logsys_clk_start(libusb_device_handle* dev, int freqKHz, bool* success);
int logsys_clk_stop(libusb_device_handle* dev, bool* was_running);

int logsys_tx_set_reset(libusb_device_handle* dev, bool reset, /*out*/bool* success);
int logsys_tx_get_reset(libusb_device_handle* dev, /*out*/bool* reset);

int logsys_tx_get_pwr_limit(libusb_device_handle* dev, /*out*/LogsysPwrLimit* limit);
int logsys_tx_set_pwr_limit(libusb_device_handle* dev, LogsysPwrLimit limit);
//correction coefficients
//TODO: parsing unimplemented!
//ch -> char[21]
int logsys_tx_get_pwr_corr(libusb_device_handle* dev, /*out*/char* ch);

int logsys_tx_set_vcc(libusb_device_handle* dev, bool vcc);
int logsys_tx_get_vcc(libusb_device_handle* dev, /*out*/ bool* vcc);

//get & set reverse current tolerance. Values in milliamps
int logsys_tx_get_rev_curr(libusb_device_handle* dev, double* mAmps);
int logsys_tx_set_rev_curr(libusb_device_handle* dev, double mAmps);

int logsys_tx_get_active_func(libusb_device_handle* dev, /*out*/LogsysFunction* func);

int logsys_tx_jtag_begin(libusb_device_handle* dev, LogsysJtagMode mode, /*out*/bool* ready);
int logsys_tx_jtag_end(libusb_device_handle* dev);

int logsys_tx_serial_begin(libusb_device_handle* dev, bool* success);
int logsys_tx_serial_end(libusb_device_handle* dev);

//Performs a JTAG boundary-scan & returns the device IDs in the `jtag_devs` array and saves the number of devices in `found_devs`
//NOTE: if there's more than `max_devs`, then only the first `max_devs` gets returned, and `found_devs` will be set to `max_devs`
int logsys_jtag_scan(libusb_device_handle* dev, /*out*/uint32_t jtag_devs[], int max_devs, /*out*/int* found_devs);
//select between Write & Compare and Write & Echo modes
int logsys_jtag_get_mode(libusb_device_handle* dev, /*out*/LogsysJtagMode* mode);
int logsys_jtag_set_mode(libusb_device_handle* dev, LogsysJtagMode mode);
//in JTAG Write & Compare mode, the development cable checks the response and this query reads the check results 
int logsys_jtag_check_error(libusb_device_handle* dev, /*out*/bool* error);

#endif //_LOGSYSDRV_USB_H
