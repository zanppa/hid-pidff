# hid-pidff
Driver managed memory model of hid-pidff support for Linux kernel

This repository contains an EXPERIMENTAL [USB HID PID driver managed mode](https://usb.org/sites/default/files/documents/pid1_01.pdf) force feedback driver for Linux.

The code is done againt latest git at beginning of 2020, but is tested on Linux 4.9.0-6-amd64.

The driver is tested ONLY with Saitek Cyborg Evo Force.

Note that the patch may be outdated, try the hid-pidff.c instead.

## Installation
Copy the hid-pidff.c to `drivers/hid/usbhid/` in your kernel tree. Build the usbhid module with

```
sudo make M=drivers/hid/usbhid
```

and install the module, e.g. using

```
sudo rmmod usbhid && sudo insmod drivers/hid/usbhid/usbhid.ko
```


## Notes
This driver is experimental and may cause issues with device managed force feedback devices or other hid devices. Even though I try to test the driver, there might be bugs or memory leaks. Try at your own risk.
