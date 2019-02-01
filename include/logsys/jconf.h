/** @file jconf.h LOGSYS JTAG functions */
#ifndef _LOGSYSDRV_JCONF_H
#define _LOGSYSDRV_JCONF_H

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
void* logsys_conv_bit2svf(char* bitfile);

#endif //_LOGSYSDRV_JCONF_H
