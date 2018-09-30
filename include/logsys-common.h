#ifndef _LOGSYSDRV_COMMON_H
#define _LOGSYSDRV_COMMON_H

typedef struct _logsys_status{
	char vOutL, vOutH, vJtagRefL, vJtagRefH, vIoRefL, vIoRefH;
	char iOutDetailL, iOutDetailH, iOutL, iOutH;
	char flagPwr, flagPin;
} LogsysStatus;

#endif //_LOGSYSDRV_STATUS_H
