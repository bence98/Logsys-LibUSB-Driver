/** @file common.h Common structures */
#ifndef _LOGSYSDRV_COMMON_H
#define _LOGSYSDRV_COMMON_H

/** Vendor ID of the LOGSYS cable */
#define LOGSYS_VID 0x03eb
/** Product ID of the LOGSYS cable */
#define LOGSYS_PID 0xf0ff

/** construct a 2-byte word from high & low bytes */
#define TO_WORD(h, l) (((h)&255)<<8|((l)&255))
/** construct a 4-byte dword from high -> low bytes */
#define TO_DWORD(h, j, k, l) (((h)&255)<<24|((j)&255)<<16|((k)&255)<<8|((l)&255))
/** deconstruct a multi-byte int to bytes
  * @param in the int
  * @param len sizeof(type)
  * @param out a char* buffer of size >= `len`
  */
#define TO_BYTES(in, len, out) {\
	for(size_t i=0;i<(len);i++)\
		(out)[i]=((in)>>(8*i))&0xff;\
}

/** Data from @link #logsys_get_status()@endlink */
typedef struct{
	char vOutL, vOutH, vJtagRefL, vJtagRefH, vIoRefL, vIoRefH;
	char iOutDetailL, iOutDetailH, iOutL, iOutH;
	char flagPwr, flagPin;
} LogsysStatus;

/** Data from @link #logsys_clk_status()@endlink */
typedef struct{
	bool active;
	char periodRegL, periodRegH, prescaler;
} LogsysClkStatus;


/** Data from @link logsys_get_pwr_limit()@endlink */
typedef enum{
	L450mA, L700mA, L950mA
} LogsysPwrLimit;

/** Data from @link #logsys_get_active_func()@endlink
  * @see LogsysJtagMode
  */
typedef enum{
	NONE, JTAG_RW, JTAG_CHK, SPI, I2C, PIC
} LogsysFunction;

/** JTAG modes */
typedef enum{
	/** Write & Echo mode: driver sends JTAG output, device sends input data back */
	MODE_ECHO=1,
	/** Write & Compare mode: driver sends output AND expected input, device responds whether it matches
	  * @see #logsys_jtag_check_error()
	  */
	MODE_CMP
} LogsysJtagMode;

typedef struct{
	char major, minor;
} LogsysFirmwareVersion;

/** Calculate clock registers for the given frequency
  * @param clkFreqHz frequency in Hz
  * @param divisor 2 for the standard clock and 4 for the Serial port clock
  * @note Does not set `active`
  */
LogsysClkStatus logsys_create_clk_status(double clkFreqHz, double divisor);

#endif //_LOGSYSDRV_COMMON_H
