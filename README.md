# LOGSYS LibUSB driver

This repository contains the Linux-compatible driver for the [LOGSYS](http://logsys.mit.bme.hu) Development Cable.

[RC1](https://github.com/bence98/Logsys-LibUSB-Driver/releases) is out and available for download!

## Dependencies

* libusb-1.0 (version 0.1 is not supported!)
* (for building) libusb-1.0-dev

## Compiling the driver

* You need to clone `libxsvf` by running `git submodule init && git submodule update`
* `make`: The shared object will be created in the `build` folder
* `sudo make install`: install the library to `/usr/local/lib`. You will probably need to do this to run the tests (though you _could technically_ use `$LD_LIBRARY_PATH` to circumvent this)
* tests: `make test`: Test binaries will be created in the `build` folder
  * `hotplug-test`: Prints 'Logsys [dis]connected' when you [un]plug the device
  * `logsys-test`: Communication protocol test. Exit with the `quit` command or with CTRL+D (EOF). Do not kill with SIGINT (CTRL+C)!
* Using the driver in your app: link with `-llogsys-drv` (assuming you installed the lib)
* Allowing an unprivileged user to access the device (recommended): run `udev-rule.sh` as root
* For a step-by-step guide, see `INSTALLING.md`

## API

See the `include` folder & the tests or the [Wiki page](https://github.com/bence98/Logsys-LibUSB-Driver/wiki/Legacy:API).

## Notes

This project is not affiliated with the LOGSYS project.

The driver is in a **testing** stage. Use with caution! No warranty.
