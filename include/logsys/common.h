#ifndef _LOGSYSDRV_COMMON_H
#define _LOGSYSDRV_COMMON_H

#define LOGSYS_VID 0x03eb
#define LOGSYS_PID 0xf0ff

/** construct a 2-byte word from high & low bytes */
#define TO_WORD(h, l) (((h)&255)<<8|((l)&255))
/** construct a 4-byte dword from high -> low bytes */
#define TO_DWORD(h, j, k, l) (((h)&255)<<24|((j)&255)<<16|((k)&255)<<8|((l)&255))

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

typedef enum{
	/** Write & Echo mode: driver sends JTAG output, device sends input data back */
	MODE_ECHO=1,
	/** Write & Compare mode: driver sends output AND expected input, device responds whether it matches */
	MODE_CMP
} LogsysJtagMode;

LogsysClkStatus logsys_create_clk_status(double clkFreqHz, double divisor);

#endif //_LOGSYSDRV_COMMON_H
