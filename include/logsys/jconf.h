#ifndef _LOGSYSDRV_JCONF_H
#define _LOGSYSDRV_JCONF_H

int logsys_jtag_dl_svf(libusb_device_handle* dev, void* f);
int logsys_jtag_dl_xsvf(libusb_device_handle* dev, void* f);
void* logsys_conv_bit2svf(char* bitfile);

#endif //_LOGSYSDRV_JCONF_H
