#ifndef _LOGSYSDRV_COMMON_H
#define _LOGSYSDRV_COMMON_H

#define LOGSYS_VID 0x03eb
#define LOGSYS_PID 0xf0ff

typedef struct _logsys_status{
	char vOutL, vOutH, vJtagRefL, vJtagRefH, vIoRefL, vIoRefH;
	char iOutDetailL, iOutDetailH, iOutL, iOutH;
	char flagPwr, flagPin;
} LogsysStatus;

typedef struct _logsys_clk_status{
	bool active;
	char periodRegL, periodRegH, prescaler;
} LogsysClkStatus;

typedef enum{
	L450mA, L700mA, L950mA
} LogsysPwrLimit;

typedef enum{
	NONE, JTAG_RW, JTAG_CHK, SPI, I2C, PIC
} LogsysFunction;

#endif //_LOGSYSDRV_COMMON_H
