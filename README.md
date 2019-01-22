# LOGSYS LibUSB driver

This repository contains the Linux-compatible driver for the [LOGSYS](http://logsys.mit.bme.hu) Development Cable.

[RC1](https://github.com/bence98/Logsys-LibUSB-Driver/releases) is out and available for download!

## Dependencies

* libusb-1.0 (version 0.1 is not supported!)
* (for building) libusb-1.0-dev
* [optional] udev (we provide a udev rule to allow an unprivileged user to use the device)

## Compiling the driver

For a step-by-step guide, see `INSTALLING.md`

## API

See the `include` folder & the tests or the [Wiki page](https://github.com/bence98/Logsys-LibUSB-Driver/wiki/Legacy:API).

## Notes

This project is not affiliated with the LOGSYS project.

The driver is in a **testing** stage. Use with caution! No warranty.
