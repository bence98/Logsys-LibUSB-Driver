#ifndef _LOGSYSDRV_USB_H
#define _LOGSYSDRV_USB_H

#include "logsys-common.h"

//interface number
#define LOGSYS_IFN_DEV 0
#define LOGSYS_IFN_COM 1
//interface descriptor
#define LOGSYS_IFD_DEV 2
#define LOGSYS_IFD_COM 3

#define LOGSYS_REQTYP_IN  0xc0
#define LOGSYS_REQTYP_OUT 0x40

#define LOGSYS_DLEN_REQ48 24

bool logsys_usb_init();
bool logsys_usb_end();
libusb_device_handle* logsys_usb_open();
void logsys_usb_close(libusb_device_handle* dev);
//don't forget to `libusb_hotplug_deregister_callback()`!
/*async*/ bool logsys_hotplug_enable(libusb_hotplug_event evt_type, libusb_hotplug_callback_fn callback, /*out*/libusb_hotplug_callback_handle* hndl);

int logsys_tx_get_status(libusb_device_handle* dev, /*out*/LogsysStatus* data);
//will trigger -EOVERFLOW (-8)
//data -> char[4]
int logsys_tx_clk_status(libusb_device_handle* dev, /*out*/LogsysClkStatus* data);
int logsys_clk_start(libusb_device_handle* dev, int freqKHz);
int logsys_clk_stop(libusb_device_handle* dev, bool* was_running);

//will trigger -EPIPE (-9)
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
//TODO: unimplemented!
int logsys_tx_det_rev_curr(libusb_device_handle* dev, double* mAmps);
int logsys_tx_set_rev_curr(libusb_device_handle* dev, double mAmps);

//I've taken a WildAssGuess as to what the parameters may be
//num_boards -> char[1]: number of JTAG devices on the chain
//jtag_dev -> char[2]: some info about the JTAG device (FPGA model?)
int logsys_tx_scan_jtag(libusb_device_handle* dev, /*out*/char* num_boards, /*out*/char* jtag_dev);

#endif //_LOGSYSDRV_USB_H
