#ifndef _LOGSYSDRV_USB_UTIL_H
#define _LOGSYSDRV_USB_UTIL_H

#define LOGSYS_VID 0x03eb
#define LOGSYS_PID 0xf0ff

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

//data -> char[12]
int logsys_tx_get_status(libusb_device_handle* dev, /*out*/char* data);
//will trigger -EOVERFLOW (-8)
//data -> char[4]
int logsys_tx_clk(libusb_device_handle* dev, /*out*/char* data);
//will trigger -EPIPE (-9)
//data -> char[1]
int logsys_tx_reset(libusb_device_handle* dev, /*out*/char* data);
//unknown operation, looks like some preamble (maybe signals whether LogSYS GUI is running?)
//sends a request#2 value=4
//ch -> char[1] (usually =0)
//buf -> char[21]
int logsys_tx_pwr_limit(libusb_device_handle* dev, /*out*/char* ch, /*out*/char* buf);
int logsys_tx_set_vcc(libusb_device_handle* dev, bool vcc);
//I've taken a WildAssGuess as to what the parameters may be
//num_boards -> char[1]: number of JTAG devices on the chain
//jtag_dev -> char[2]: some info about the JTAG device (FPGA model?)
int logsys_tx_scan_jtag(libusb_device_handle* dev, /*out*/char* num_boards, /*out*/char* jtag_dev);

#endif //_LOGSYSDRV_USB_UTIL_H
