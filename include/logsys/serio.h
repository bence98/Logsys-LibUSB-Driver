/** @file serio.h LOGSYS I/O functions (Serial, USART etc.) */
#ifndef _LOGSYSDRV_SERIO_H
#define _LOGSYSDRV_SERIO_H

#include "logsys/common.h"

/** The state of all pins on rising/falling edge of the clock */
typedef struct{
	char rising, falling;
} LogsysSerialLines;

/** Bit pattern constants for the serial pins */
typedef enum{
	MISO=0x04,
	MOSI=0x08,
	RESET=0x10,
	SCLK=0x20
} LogsysSerialPin;

/** USART status codes. Currently only one is known. */
typedef enum{
	USART_OK
} LogsysUsartStatus;

/** Sends and receives BitBang I/O data */
int logsys_serial_send(libusb_device_handle* dev, LogsysSerialLines send, LogsysSerialLines* recv);

/** Gets data from USART
  * @param dev LOGSYS device
  * @param buf a byte array
  * @param maxlen length of `buf`
  * @param len number of bytes returned
  * @param status transfer status
  */
int logsys_usart_getstr(libusb_device_handle* dev, char* buf, int maxlen, /*out*/int* len, /*out*/LogsysUsartStatus* status);

/** Prints data to USART
  * @param dev LOGSYS device
  * @param buf a byte array
  * @param len length of `buf`
  */
int logsys_usart_putstr(libusb_device_handle* dev, char* buf, int len);

#endif //_LOGSYSDRV_SERIO_H
