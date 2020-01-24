# hid-pidff
Driver managed memory model of hid-pidff support for Linux kernel

This repository contains an EXPERIMENTAL [USB HID PID driver managed mode](https://usb.org/sites/default/files/documents/pid1_01.pdf) force feedback driver for Linux.

The code is done againt latest git at beginning of 2020, but is tested on Linux 4.9.0-6-amd64.

The driver is made to provide force-feedback support for Saitek Cyborg Evo Force. The driver is ONLY tested on this joystick.

Note that the patch may be outdated, use the `hid-pidff.c` instead.

## Installation
Copy the `hid-pidff.c` to `drivers/hid/usbhid/` in your kernel tree. Build the usbhid module from the kernel source root with

```
sudo make M=drivers/hid/usbhid
```

and load the module, e.g. by using

```
sudo rmmod usbhid && sudo insmod drivers/hid/usbhid/usbhid.ko
```


## Notes
This driver is experimental and may cause issues with device managed force feedback devices or other hid devices. Even though I try to test the driver, there might be bugs or memory leaks. Try at your own risk.

On Windows the Saitek Cyborg Evo Force uses I-Force protocol. [There are](https://patchwork.kernel.org/patch/69106/) [some](https://patchwork.kernel.org/patch/68558/) [patches](https://patchwork.kernel.org/patch/69092/) that might also work for the joystick.
