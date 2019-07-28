#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <string.h>
#include "logsys/usb.private.h"
#include "logsys/serio.h" 

int logsys_serial_begin(libusb_device_handle* dev, bool* success){
	LogsysClkStatus status=logsys_create_clk_status(10, 4);
	int resp=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 96, TO_WORD(status.periodRegH, status.periodRegL), status.prescaler, (char*)success, 1, 0);
	if(resp<0) return resp;
	if(*success){
		resp=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 98, 0, 0, NULL, 0, 0);
	}
	return resp;
}

int logsys_serial_change_clk(libusb_device_handle* dev, int freqHz){
	LogsysClkStatus status=logsys_create_clk_status(freqHz, 4);
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 99, TO_WORD(status.periodRegH, status.periodRegL), status.prescaler, NULL, 0, 0);
}

int logsys_serial_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 97, 0, 0, NULL, 0, 0);
}

int logsys_serial_send(libusb_device_handle* dev, LogsysSerialLines send, LogsysSerialLines* recv){
	int res=libusb_bulk_transfer(dev, LOGSYS_OUT_EP3, (char*)&send, 2, NULL, 0);
	if(res<0) return res;
	return libusb_bulk_transfer(dev, LOGSYS_IN_EP4, (char*)recv, 2, NULL, 0);
}

int logsys_usart_get_caps(libusb_device_handle* dev, LogsysUsartCaps* capabilities){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 48, 0, 0, (char*)capabilities, sizeof(LogsysUsartCaps), 0);
}

int logsys_usart_begin(libusb_device_handle* dev, unsigned int baud, bool usrt, LogsysUsartEncoding enc, LogsysUsartParity parity, bool* success){
	if(baud<=30||baud>2000000) return -2;
	int res=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 49, usrt, 0, (char*)success, 1, 0);
	if(res<0) return res;
	// Calculating wValue
	// It looks like a clock register with 1/8 prescaler
	static int mcuFreqHz=16000000; //16 MHz
	unsigned short wValue=(mcuFreqHz/8/baud)-1;
	res=libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 53, wValue, 0, NULL, 0, 0);
	if(res<0) return res;
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 54, enc, parity, NULL, 0, 0);
}

int logsys_usart_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 50, 0, 0, NULL, 0, 0);
}

int logsys_usart_getstr(libusb_device_handle* dev, char* buf, int maxlen, int* len, LogsysUsartStatus* status){
	char tmp[maxlen+1];
	int res=libusb_interrupt_transfer(dev, LOGSYS_IN_EP6, tmp, maxlen+1, len, 0);
	if(res>=0){
		memcpy(buf, tmp, --(*len));
		*status=tmp[*len];
	}
	return res;
}

int logsys_usart_putstr(libusb_device_handle* dev, char* buf, int len){
	return libusb_bulk_transfer(dev, LOGSYS_OUT_EP5, buf, len, NULL, 0);
}

int logsys_spi_begin(libusb_device_handle* dev, LogsysSpiSpeed freq, LogsysSpiMode mode, bool* success){
	int res=libusb_control_transfer(dev, LOGSYS_REQTYP_IN, 32, (1<<freq)-1, mode, (char*)success, 1, 0);
	if(res<0) return res;
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 34, 0, 0, NULL, 0, 0);
}

int logsys_spi_end(libusb_device_handle* dev){
	return libusb_control_transfer(dev, LOGSYS_REQTYP_OUT, 33, 0, 0, NULL, 0, 0);
}
