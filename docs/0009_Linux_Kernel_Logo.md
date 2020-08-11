# Linux Kernel Logo

Linux内核Logo图片

## make menuconfig

* CONFIG_DRM_FBDEV_EMULATION
* CONFIG_FRAMEBUFFER_CONSOLE
* CONFIG_VT

## Shell generate ppm

```sh
#!/bin/sh
pngtopnm 001.png > car.pnm
pnmquant 224 car.pnm > car224.pnm
pnmtoplainpnm car224.pnm > logo_car_clut224.ppm

cp logo_car_clut224.ppm $android/vendor/nxp-opensource/kernel_imx/drivers/video/logo/
```