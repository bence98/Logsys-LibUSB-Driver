# Installing the LOGSYS driver

## Installing: 64-bit Debian / Ubuntu / Linux Mint etc.
Download the DEB file and install it with `dpkg` or your favourite package installer.

Now, you should be able to run `logsys-test` from the terminal.

## Installing: Arch Linux 64-bit (for lazy people :P )
Download and install the TAR XZ using `pacman -U`

## Installing: Arch Linux, from source (cross-platform)
You will need to use `makepkg`, similar to how you would use the AUR

* `git clone https://git.sch.bme.hu/bence98/logsys-arch-pkg.git`
* `cd logsys-arch-pkg && makepkg -i`

## Installing: other Linux distros
You will have to compile the driver yourself

* Before you begin, you need to make sure you have `libusb-1.0` and `libusb-1.0-dev` installed on your system.
* First, clone `libxsvf`, by running `git submodule init && git submodule update`
* Then, run `make` and `sudo make install`
* It is advised to run `sudo ./udev-rule.sh`
* Finally, you can run `make test` to get the `logsys-test` binary in the `build` folder

## Installing: macOS
First, you're very brave. I like that :D Second, you should *really* have a package manager (I would recommend [Homebrew](https://brew.sh/))

* Before you begin, you need to install LibUSB 1.0 (for example, with Homebrew it would mean running `brew install libusb`)
* First, clone `libxsvf`, by running `git submodule sync && git submodule update`
* Then, run `make` and `sudo make install`
* Finally, you can run `make test` to get the `logsys-test` binary in the `build` folder

Warning: as of yet, there was limited testing done on macOS.
