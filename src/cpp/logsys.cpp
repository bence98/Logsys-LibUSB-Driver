#include "logsyspp/logsys.hpp"
#include "logsyspp/jtag.hpp"
#include <stdexcept>

extern "C"{
#include "logsys/control.h"
#include "logsys/usb.h"
}

bool LogsysClock::start(int freqHz) const{
	bool ret;
	logsys_clk_start(this->cable.dev, freqHz, &ret);
	return ret;
}

bool LogsysClock::stop(){
	bool ret;
	logsys_clk_stop(this->cable.dev, &ret);
	return ret;
}

bool LogsysReset::set(bool val){
	bool ret;
	logsys_set_reset(this->cable.dev, val, &ret);
	return ret;
}

bool LogsysReset::get() const{
	bool ret;
	logsys_get_reset(this->cable.dev, &ret);
	return ret;
}

void LogsysVcc::set(bool val){
	logsys_set_vcc(this->cable.dev, val);
}

bool LogsysVcc::get(){
	bool ret;
	logsys_get_vcc(this->cable.dev, &ret);
	return ret;
}

LogsysDownloadCable::LogsysDownloadCable(libusb_context* ctx, libusb_device* pDev) : dev(logsys_usb_open(ctx, pDev)){
	if(!dev)
		throw std::invalid_argument("Open failed!");
}

LogsysDownloadCable::~LogsysDownloadCable(){
	logsys_usb_close(dev);
}

LogsysClock LogsysDownloadCable::clk(){
	return LogsysClock(*this);
}

LogsysReset LogsysDownloadCable::rst(){
	return LogsysReset(*this);
}

LogsysVcc LogsysDownloadCable::vcc(){
	return LogsysVcc(*this);
}

LogsysJtag LogsysDownloadCable::jtag(LogsysJtagMode mode){
	return LogsysJtag(*this, mode);
}
