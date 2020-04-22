/** @file jconf.h LOGSYS JTAG functions */
#ifndef _LOGSYSDRV_JCONF_H
#define _LOGSYSDRV_JCONF_H

/** Begin JTAG transmission */
int logsys_jtag_begin(libusb_device_handle* dev, LogsysJtagMode mode, /*out*/bool* ready);
/** End JTAG transmission */
int logsys_jtag_end(libusb_device_handle* dev);

/** Performs a JTAG boundary-scan & returns the device IDs in the `jtag_devs` array and saves the number of devices in `found_devs`
  * @note if there's more than `max_devs`, then only the first `max_devs` gets returned, and `found_devs` will be set to `max_devs`
  */
int logsys_jtag_scan(libusb_device_handle* dev, /*out*/uint32_t jtag_devs[], int max_devs, /*out*/int* found_devs);
/** Gets current JTAG mode */
int logsys_jtag_get_mode(libusb_device_handle* dev, /*out*/LogsysJtagMode* mode);
/** Selects between @link MODE_CMP Write & Compare mode@endlink and @link MODE_ECHO Write & Echo@endlink modes */
int logsys_jtag_set_mode(libusb_device_handle* dev, LogsysJtagMode mode);
/** In JTAG @link MODE_CMP Write & Compare mode@endlink, this query reads the check results */
int logsys_jtag_check_error(libusb_device_handle* dev, /*out*/bool* error);

/** Download SVF file to the connected FPGA
  * @param dev LOGSYS device
  * @param f an open FILE* pointer to the SVF file
  */
int logsys_jtag_dl_svf(libusb_device_handle* dev, void* f);
/** Download XSVF file to the connected FPGA
  * @param dev LOGSYS device
  * @param f an open FILE* pointer to the SVF file
  */
int logsys_jtag_dl_xsvf(libusb_device_handle* dev, void* f);
/** Convert a BIT/JED file to SVF
  * @param bitfile the path to the BIT file
  * @returns an open FILE* pointer to the SVF file
  */
void* logsys_conv_bit2svf(const char* bitfile);

#endif //_LOGSYSDRV_JCONF_H
