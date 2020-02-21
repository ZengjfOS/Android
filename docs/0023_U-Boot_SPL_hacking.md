# U-Boot SPL hacking

## 参考文档

* [Uboot 2017.01 启动流程分析](https://blog.csdn.net/kl1125290220/article/details/78683999)

## Linker

* arch/arm/cpu/armv8/u-boot-spl.lds

## SPL U-Boot function call route

* arch/arm/cpu/armv8/start.S
  ```S
  [...省略]
  _start:
      b       reset
  [...省略]
  reset:
      bl      _main
  [...省略]
  ```
  * arch/arm/lib/crt0_64.S
    ```S
    ENTRY(_main)
        [...省略]
        bl      board_init_f
        [...省略]
        b       board_init_r                    /* PC relative jump */
        [...省略]
    ENDPROC(_main)
    ```
    * common/Makefile
      ```Makefile
      [...省略]
      ifndef CONFIG_SPL_BUILD                                   <--------- SPL阶段不使用这里的代码，第二阶段的时候使用这里的代码
      [...省略]
      # # boards
      obj-y += board_f.o
      obj-y += board_r.o
      [...省略]
      endif # !CONFIG_SPL_BUILD
      [...省略]
      ```
      * common/board_f.c
        * void board_init_f(ulong boot_flags)
      * common/board_r.c
        * void board_init_r(gd_t *new_gd, ulong dest_addr)
    * board/freescale/imx8qm_mek/Makefile
      ```Makefile
      #
      # Copyright 2017 NXP
      #
      # SPDX-License-Identifier:      GPL-2.0+
      #
      
      obj-y += imx8qm_mek.o
      
      ifdef CONFIG_SPL_BUILD                                    <----------- 使用这里的SPL代码
      obj-y += spl.o
      endif
      ```
      * board/freescale/imx8qm_mek/spl.c
        * void board_init_f(ulong dummy)
    * common/spl/Makefile
      ```Makefile
      [...省略]
      ifdef CONFIG_SPL_BUILD                                    <----------- 使用这里的SPL代码
      obj-$(CONFIG_SPL_FRAMEWORK) += spl.o
      obj-$(CONFIG_$(SPL_TPL_)BOOTROM_SUPPORT) += spl_bootrom.o
      obj-$(CONFIG_$(SPL_TPL_)LOAD_FIT) += spl_fit.o
      obj-$(CONFIG_$(SPL_TPL_)NOR_SUPPORT) += spl_nor.o
      obj-$(CONFIG_$(SPL_TPL_)XIP_SUPPORT) += spl_xip.o
      obj-$(CONFIG_$(SPL_TPL_)YMODEM_SUPPORT) += spl_ymodem.o
      [...省略]
      ```
      * common/spl/spl.c
        * void board_init_r(gd_t *dummy1, ulong dummy2)


## Normal U-Boot function call route

* arch/arm/cpu/armv8/start.S
  ```S
  [...省略]
  _start:
      b       reset
  [...省略]
  reset:
      bl      _main
  [...省略]
  ```
  * arch/arm/lib/crt0_64.S
    ```S
    ENTRY(_main)
        [...省略]
        bl      board_init_f
        [...省略]
        b       board_init_r                    /* PC relative jump */
        [...省略]
    ENDPROC(_main)
    ```
    * common/Makefile
      ```Makefile
      [...省略]
      ifndef CONFIG_SPL_BUILD                                   <--------- 第二阶段的时候使用这里的代码
      [...省略]
      # # boards
      obj-y += board_f.o                                        <--------- 第二阶段使用这里的board_init_f代码
      obj-y += board_r.o                                        <--------- 第二阶段使用这里的board_init_r代码
      [...省略]
      endif # !CONFIG_SPL_BUILD
      [...省略]
      ```
      * common/board_f.c
        * void board_init_f(ulong boot_flags)
      * common/board_r.c
        * void board_init_r(gd_t *new_gd, ulong dest_addr)
    * board/freescale/imx8qm_mek/Makefile
      ```Makefile
      #
      # Copyright 2017 NXP
      #
      # SPDX-License-Identifier:      GPL-2.0+
      #
      
      obj-y += imx8qm_mek.o
      
      ifdef CONFIG_SPL_BUILD                                    <----------- 第二阶段不使用这里的SPL代码
      obj-y += spl.o
      endif
      ```
      * board/freescale/imx8qm_mek/spl.c
        * void board_init_f(ulong dummy)
    * common/spl/Makefile
      ```Makefile
      [...省略]
      ifdef CONFIG_SPL_BUILD                                    <----------- 第二阶段不使用这里的SPL代码
      obj-$(CONFIG_SPL_FRAMEWORK) += spl.o
      obj-$(CONFIG_$(SPL_TPL_)BOOTROM_SUPPORT) += spl_bootrom.o
      obj-$(CONFIG_$(SPL_TPL_)LOAD_FIT) += spl_fit.o
      obj-$(CONFIG_$(SPL_TPL_)NOR_SUPPORT) += spl_nor.o
      obj-$(CONFIG_$(SPL_TPL_)XIP_SUPPORT) += spl_xip.o
      obj-$(CONFIG_$(SPL_TPL_)YMODEM_SUPPORT) += spl_ymodem.o
      [...省略]
      ```
      * common/spl/spl.c
        * void board_init_r(gd_t *dummy1, ulong dummy2)

## include

* include/configs/imx8qm_mek.h

## configs

* configs/imx8qm_mek_androidauto_defconfig

## BSP U-Boot compile

* build/target/board/Android.mk
  * device/fsl/imx8q/mek_8q/AndroidBoard.mk
    ```Makefile
    LOCAL_PATH := $(call my-dir)

    include device/fsl/common/build/kernel.mk
    include device/fsl/common/build/uboot.mk
    include device/fsl/common/build/dtbo.mk
    include device/fsl/common/build/imx-recovery.mk
    include device/fsl/common/build/gpt.mk
    include $(LOCAL_PATH)/AndroidUboot.mk
    include $(FSL_PROPRIETARY_PATH)/fsl-proprietary/media-profile/media-profile.mk
    ifneq ($(PRODUCT_IMX_CAR),true)
    include $(FSL_PROPRIETARY_PATH)/fsl-proprietary/sensor/fsl-sensor.mk
    endif
    ```
    * device/fsl/imx8q/mek_8q/AndroidUboot.mk
      * vendor/nxp-opensource/imx-mkimage/iMX8QM/scripts/android.mak

## 简要信息

dom0 u-boot: `configs/imx8qm_mek_spl_defconfig`

## boot.scr

* ** Unable to read file boot.scr **
* include/configs/imx8qm_mek.h
  ```
  "script=boot.scr\0" \
  ```