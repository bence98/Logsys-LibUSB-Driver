#ifndef _LOGSYSDRV_LOGSYS_HPP
#define _LOGSYSDRV_LOGSYS_HPP

extern "C"{
#include <libusb-1.0/libusb.h>
#include "logsys/common.h"
}

// Forward declaration
class LogsysDownloadCable;
// External declarations
class LogsysJtag;

// Peripherals
class LogsysClock{
	friend class LogsysDownloadCable;
	
	LogsysDownloadCable& cable;
	LogsysClock(LogsysDownloadCable& cable) : cable(cable) {}
	
	public:
	bool start(int freqHz) const;
	bool stop();
	int getFreq();
};

class LogsysReset{
	friend class LogsysDownloadCable;
	
	LogsysDownloadCable& cable;
	LogsysReset(LogsysDownloadCable& cable) : cable(cable) {}
	
	public:
	bool set(bool);
	bool get() const;
	
	operator bool() const{
		return get();
	}
	
	LogsysReset& operator=(bool b){
		set(b);
		return *this;
	}
};

class LogsysVcc{
	friend class LogsysDownloadCable;
	
	LogsysDownloadCable& cable;
	LogsysVcc(LogsysDownloadCable& cable) : cable(cable) {}
	
	public:
	void set(bool);
	bool get();
	
	operator bool(){
		return get();
	}
	
	LogsysVcc& operator=(bool b){
		set(b);
		return *this;
	}
};

// Main cable object
class LogsysDownloadCable{
	friend class LogsysClock;
	friend class LogsysReset;
	friend class LogsysVcc;
	friend class LogsysJtag;
	
	libusb_device_handle* dev;
	
	public:
	LogsysDownloadCable(libusb_context* ctx, libusb_device* pDev);
	virtual ~LogsysDownloadCable();
	
	LogsysStatus getStatus();
	
	LogsysClock clk();
	LogsysReset rst();
	LogsysVcc vcc();
	
	LogsysJtag jtag(LogsysJtagMode mode);
};

#endif //_LOGSYSDRV_LOGSYS_HPP
