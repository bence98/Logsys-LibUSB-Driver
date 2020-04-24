/** @file usb.h LOGSYS USB functions */
#ifndef _LOGSYSDRV_USB_H
#define _LOGSYSDRV_USB_H

#include "logsys/common.h"

/** Open the USB device (i.e. a LOGSYS Download Cable)
  * @note pDev may be NULL, in which case we try to find a device
  * @warning You should have initialized the LibUSB context before calling any LOGSYS functions!
  * @returns a LibUSB handle, or NULL on error
  */
libusb_device_handle* logsys_usb_open(libusb_device* pDev);
/** Release and close the LOGSYS device */
void logsys_usb_close(libusb_device_handle* dev);
/** Register a hotplug listener for LOGSYS devices
  * @param evt_type Type of event: connect/disconnect
  * @param callback The handler to call on a hotplug event
  * @param hndl The event handle
  * @warning Don't forget to call `libusb_hotplug_deregister_callback()`!
  * @note You will need to periodically call `libusb_handle_events_completed()` in the main thread
  */
bool logsys_hotplug_enable(libusb_hotplug_event evt_type, libusb_hotplug_callback_fn callback, /*out*/libusb_hotplug_callback_handle* hndl);

#endif //_LOGSYSDRV_USB_H
