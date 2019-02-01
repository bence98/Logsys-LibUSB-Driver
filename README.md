# LOGSYS LibUSB driver

This repository contains the Linux-compatible/cross-platform<sup>1</sup> driver for the [LOGSYS](http://logsys.mit.bme.hu) Development Cable.

[RC2](https://github.com/bence98/Logsys-LibUSB-Driver/releases) is out and available for download!

## Features
* Reading device status
* Setting VCC, RESET, CLK pins
* Hotplugging event registration
* Configuring the FPGA using SVF, XSVF, BIT (needs XILINX) or JED (needs XILINX; untested) files
* Crude USART support (no `tty`!)
* BitBang I/O using the MISO & MOSI pins

## Issues
* No SPI/I2C/PIC support

## Dependencies

* libusb-1.0 (version 0.1 is not supported!)
* (for building) libusb-1.0-dev
* [optional] udev (we provide a udev rule to allow an unprivileged user to use the device)

## Compiling the driver

For a step-by-step guide, see `INSTALLING.md`

## API

See the `include` folder & the tests or the [Wiki page](https://github.com/bence98/Logsys-LibUSB-Driver/wiki/Legacy:API).

## Notes

The driver is in a **testing** stage. Use with caution! No warranty.

<sup>1</sup>: The driver works on theoretically any POSIX-compliant OS with LibUSB on it

* macOS: Driver compiles & works, but lacks BIT format support, due to XILINX not being available.
* Haiku OS: same state as macOS; also it is available in pre-built HPKG on x86-64bit.
* Windows MinGW: JTAG library binding does not compile, the rest of the driver seems to work. You need special software to install the SYS file of LibUSB.
