# LOGSYS LibUSB driver

This repository contains the Linux-compatible driver for the [LOGSYS](http://logsys.mit.bme.hu) Development Cable.

## Dependencies

* libusb-1.0 (version 0.1 is not supported!)
* (for building) libusb-1.0-dev

## Compiling the driver

* `make`: The shared object will be created in the `build` folder
* tests: `make test`: Test binaries will be created in the `build` folder
  * `hotplug-test`: Prints 'Logsys [dis]connected' when you [un]plug the device
  * `logsys-test`: Communication protocol test. Commands consist of `124+-` characters. Exit with `q`, then ENTER
* Using the driver in your app: In this stage, I wouldn't recommend. But if you really wanted to, just link against `logsys-drv.so`

## API

See the `include` folder & the tests. More documentation coming soon.

## Notes

This project is not affiliated with the LOGSYS project.

The driver is in an **unstable** stage. Use with caution! No warranty.
