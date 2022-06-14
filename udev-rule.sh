#!/bin/sh
echo "Installing UDEV rule... $1"
cat <<EOF >$1/lib/udev/rules.d/90-logsys.rules
#Logsys Dev Cable udev rules
#Part of the Logsys LibUSB driver

#Dev cable
SUBSYSTEM=="usb", DRIVER=="usb", ATTR{idProduct}=="f0ff", ATTR{idVendor}=="03eb", ATTR{manufacturer}=="Logsys", MODE="0777"
EOF
if [ -z "$1" ]
then
	udevadm control --reload
	echo "UDEV rule installed. You will need to unplug the device if already connected"
else
	echo "UDEV rule installed to $1"
fi
