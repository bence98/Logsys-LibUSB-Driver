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

/** USART status codes */
typedef enum{
	USART_OK,
	USART_FRAME_ERROR,
	USART_DATA_OVERFLOW_ERROR,
	USART_PARITY_ERROR=4,
	USART_EP_OVERRUN_ERROR=8
} LogsysUsartStatus;

/** USART capabilities */
typedef struct{
	char clkFreqL, clkFreqK, clkFreqJ, clkFreqH, clkPrescale;
	char minBaudL, minBaudK, minBaudJ, minBaudH;
	char maxBaudL, maxBaudK, maxBaudJ, maxBaudH;
	/* unknown */
	char data[9];
	/* not sure if bug, but this was in MSB-first order */
	char reqVerH, reqVerL;
} LogsysUsartCaps;

/** USART line encoding */
typedef enum{
	DATA_BITS_5, DATA_BITS_6, DATA_BITS_7, DATA_BITS_8, DATA_BITS_9,
	/** Two STOP bits instead of 1 */
	STOP_BITS_2=8
} LogsysUsartEncoding;

/** USART parity setting */
typedef enum{
	PARITY_NONE, PARITY_EVEN, PARITY_ODD
} LogsysUsartParity;

/** Sends and receives BitBang I/O data */
int logsys_serial_send(libusb_device_handle* dev, LogsysSerialLines send, LogsysSerialLines* recv);

/** Get USART capabilities */
int logsys_usart_get_caps(libusb_device_handle* dev, /*out*/LogsysUsartCaps* capabilities);

/** Begin USART */
int logsys_tx_usart_begin(libusb_device_handle* dev, unsigned int baud, bool usrt, LogsysUsartEncoding enc, LogsysUsartParity parity, /*out*/bool* success);
/** End USART */
int logsys_tx_usart_end(libusb_device_handle* dev);

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
