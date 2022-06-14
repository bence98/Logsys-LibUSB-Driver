#ifndef _LOGSYSDRV_JTAG_HPP
#define _LOGSYSDRV_JTAG_HPP

#include "logsyspp/logsys.hpp"
#include <list>
#include <string>
#include <cstdint>

class LogsysMode{
	friend class LogsysJtag;
	
	LogsysJtag& jtag;
	LogsysMode(LogsysJtag& jtag) : jtag(jtag) {}
	
	public:
	operator LogsysJtagMode();
	
	LogsysMode& operator =(const LogsysJtagMode& mode);
};

class LogsysJtag{
	friend class LogsysDownloadCable;
	
	LogsysDownloadCable& cable;
	LogsysJtag(LogsysDownloadCable& cable, LogsysJtagMode mode);
	
	public:
	~LogsysJtag();
	std::list<uint32_t> scan(int max_devs = 32);
	void setMode(LogsysJtagMode mode);
	LogsysJtagMode getMode();
	LogsysMode mode();
	
	// Configuration downloads
	void svf (const std::string& path);
	void xsvf(const std::string& path);
	void bit (const std::string& path);
};

#endif //_LOGSYSDRV_JTAG_HPP
