//#include <cstdio>
#include <iostream>
#include "logsyspp/logsys.hpp"
#include "logsyspp/jtag.hpp"

template<class T>
std::ostream& operator<<(std::ostream& s, const std::list<T>& v){
	for(auto&& e:v)
		s << e << ' ';
	return s;
}

int main(int argc, char* argv[]){
	std::cout << "Logsys++ layer test." << std::endl;
	if(libusb_init(NULL)!=0){
		std::cerr << "LibUSB error" << std::endl;
		return 1;
	}
	
	LogsysDownloadCable ldc(NULL, NULL);
	ldc.vcc()=true;
	
	LogsysJtag jtag=ldc.jtag(MODE_ECHO);
	std::list<uint32_t> devs=jtag.scan();
	std::cout << "Devices: " << std::hex << devs << std::endl;
	
	if(!devs.empty()&&argc>1){
		jtag.bit(argv[1]);
	}
}
