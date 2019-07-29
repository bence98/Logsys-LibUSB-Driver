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

/** SPI clock frequency */
typedef enum{
	SPI_8MHZ,
	SPI_4MHZ,
	SPI_2MHZ,
	SPI_1MHZ
} LogsysSpiSpeed;

/** SPI mode (unknown) */
typedef int LogsysSpiMode;

/** SPI commands (mostly for SPI flash chips) */
typedef enum{
	/** Write status register */
	SPI_WR_STATUS_REG=1,
	/** Page program */
	SPI_PAGE_PROG,
	/** Read data from flash */
	SPI_RD,
	/** Disable writing */
	SPI_WR_DIS,
	/** Read status register */
	SPI_RD_STATUS_REG,
	/** Enable writing */
	SPI_WR_EN,
	/** Erase a 4k block */
	SPI_ERASE_4K=32,
	/** Unprotect sector */
	SPI_SECT_UNPROT=57,
	/** Read JEDEC ID of the chip */
	SPI_RD_ID=159,
	/** Erase chip */
	SPI_CLEAR=199,
	/** Erase a 64k block */
	SPI_ERASE_64K=216
} LogsysSpiCmd;

/** Begin BitBang Serial I/O */
int logsys_serial_begin(libusb_device_handle* dev, bool* success);
/** Change BitBang Serial I/O clock frequency */
int logsys_serial_change_clk(libusb_device_handle* dev, int freqHz);
/** End BitBang Serial I/O */
int logsys_serial_end(libusb_device_handle* dev);

/** Sends and receives BitBang I/O data */
int logsys_serial_send(libusb_device_handle* dev, LogsysSerialLines send, LogsysSerialLines* recv);

/** Get USART capabilities */
int logsys_usart_get_caps(libusb_device_handle* dev, /*out*/LogsysUsartCaps* capabilities);

/** Begin USART */
int logsys_usart_begin(libusb_device_handle* dev, unsigned int baud, bool usrt, LogsysUsartEncoding enc, LogsysUsartParity parity, /*out*/bool* success);
/** End USART */
int logsys_usart_end(libusb_device_handle* dev);

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

/** Begin SPI */
int logsys_spi_begin(libusb_device_handle* dev, LogsysSpiSpeed freq, LogsysSpiMode mode, bool* success);
/** End SPI */
int logsys_spi_end(libusb_device_handle* dev);

/** Send SPI command */
int logsys_spi_cmd(libusb_device_handle* dev, LogsysSpiCmd cmd, char* wrBuf, int wrLen, /*out*/char* rdBuf, int rdLen, char* status);

#endif //_LOGSYSDRV_SERIO_H
