# Installing the LOGSYS driver

## Installing: Debian / Ubuntu / Linux Mint etc.
Download the DEB file and install it with `dpkg` or your favourite package installer.

Now, you should be able to run `logsys-test` from the terminal.

## Installing: other Linux distros
You will have to compile the driver yourself

* Before you begin, you need to make sure you have `libusb-1.0` and `libusb-1.0-dev` installed on your system.
* First, clone the `libxsvf` SVN repo, as stated in `README.md`
* Then, run `make` and `sudo make install`
* It is advised to run `sudo ./udev-rule.sh`
* Finally, you can run `make test` to get the `logsys-test` binary in the `build` folder

## Installing: macOS
First, you're very brave. I like that :D Second, you should *really* have a package manager (I would recommend [Homebrew](https://brew.sh/))

* Before you begin, you need to install LibUSB 1.0 (for example, with Homebrew it would mean running `brew install libusb`)
* First, clone the `libxsvf` SVN repo, as stated in `README.md`
* Then, run `make` and `sudo make install`
* Finally, you can run `make test` to get the `logsys-test` binary in the `build` folder

Warning: as of yet, there was limited testing done on macOS.
