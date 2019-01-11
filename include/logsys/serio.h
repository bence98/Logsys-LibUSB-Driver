#ifndef _LOGSYSDRV_SERIO_H
#define _LOGSYSDRV_SERIO_H

#include "logsys/common.h"

/** The state of all pins on rising/falling edge of the clock */
typedef struct _logsys_serial_ln{
	char rising, falling;
} LogsysSerialLines;

/** Bit pattern constants for the serial pins */
typedef enum{
	MISO=0x04,
	MOSI=0x08,
	RESET=0x10,
	SCLK=0x20
} LogsysSerialPin;

int logsys_serial_send(libusb_device_handle* dev, LogsysSerialLines send, LogsysSerialLines* recv);

int logsys_usart_getstr(libusb_device_handle* dev, char* buf, int maxlen, /*out*/int* len);
int logsys_usart_putstr(libusb_device_handle* dev, char* buf, int len);

#endif //_LOGSYSDRV_SERIO_H
