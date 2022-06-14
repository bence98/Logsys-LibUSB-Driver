#include "logsyspp/jtag.hpp"
#include <stdexcept>
#include <cstdio>

extern "C"{
#include "logsys/jconf.h"
}

LogsysMode::operator LogsysJtagMode(){
	return jtag.getMode();
}

LogsysMode& LogsysMode::operator =(const LogsysJtagMode& mode){
	jtag.setMode(mode);
	return *this;
}

LogsysJtag::LogsysJtag(LogsysDownloadCable& cable, LogsysJtagMode mode) : cable(cable) {
	bool ready=false;
	int ret=logsys_jtag_begin(this->cable.dev, mode, &ready);
	if(!ready)
		throw std::runtime_error("JTAG not ready!");
}

LogsysJtag::~LogsysJtag(){
	logsys_jtag_end(this->cable.dev);
}

std::list<uint32_t> LogsysJtag::scan(int max_devs){
	uint32_t devs[max_devs];
	int dev_len;
	int res=logsys_jtag_scan(this->cable.dev, devs, max_devs, &dev_len);
	if(res)
		throw std::runtime_error("Could not scan!");
	return std::list<uint32_t>(devs, devs+dev_len);
}

void LogsysJtag::setMode(LogsysJtagMode mode){
	logsys_jtag_set_mode(this->cable.dev, mode);
}

LogsysJtagMode LogsysJtag::getMode(){
	LogsysJtagMode mode;
	logsys_jtag_get_mode(this->cable.dev, &mode);
	return mode;
}

LogsysMode LogsysJtag::mode(){
	return LogsysMode(*this);
}

void LogsysJtag::svf(const std::string& path){
	FILE* f=fopen(path.c_str(), "r");
	logsys_jtag_dl_svf(this->cable.dev, f);
	fclose(f);
}

void LogsysJtag::xsvf(const std::string& path){
	FILE* f=fopen(path.c_str(), "r");
	logsys_jtag_dl_xsvf(this->cable.dev, f);
	fclose(f);
}

void LogsysJtag::bit(const std::string& path){
	FILE* f=(FILE*) logsys_conv_bit2svf(path.c_str());
	logsys_jtag_dl_svf(this->cable.dev, f);
	fclose(f);
}
