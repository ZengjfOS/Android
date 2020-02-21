# Android GPT

## bpt tool

https://android.googlesource.com/platform/system/tools/bpt/+/refs/heads/master/README

**一定要在Linux系统下使用**

## GPT 分区信息

* `device/fsl/imx8q/mek_8q/BoardConfig.mk`
  ```
  [...省略]
  # Support gpt
  ifeq ($(PRODUCT_IMX_CAR),true)
  BOARD_BPT_INPUT_FILES += device/fsl/common/partition/device-partitions-13GB-ab-dual-bootloader.bpt
  ADDITION_BPT_PARTITION = partition-table-7GB:device/fsl/common/partition/device-partitions-7GB-ab-dual-bootloader.bpt \
                           partition-table-28GB:device/fsl/common/partition/device-partitions-28GB-ab-dual-bootloader.bpt
  else
  BOARD_BPT_INPUT_FILES += device/fsl/common/partition/device-partitions-13GB-ab.bpt
  ADDITION_BPT_PARTITION = partition-table-7GB:device/fsl/common/partition/device-partitions-7GB-ab.bpt \
                           partition-table-28GB:device/fsl/common/partition/device-partitions-28GB-ab.bpt
  endif
  
  [...省略]
  ```
* `device/fsl/imx8q/mek_8q/AndroidBoard.mk`
  ```
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
* `device/fsl/common/build/gpt.mk`
  ```
  # Copyright (C) 2018 The Android Open Source Project
  #
  # Licensed under the Apache License, Version 2.0 (the "License");
  # you may not use this file except in compliance with the License.
  # You may obtain a copy of the License at
  #
  #      http://www.apache.org/licenses/LICENSE-2.0
  #
  # Unless required by applicable law or agreed to in writing, software
  # distributed under the License is distributed on an "AS IS" BASIS,
  # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  # See the License for the specific language governing permissions and
  # limitations under the License.
  
  ifdef BOARD_BPT_INPUT_FILES
  
  IMX_BUILT_BPTIMAGE_TARGET := $(PRODUCT_OUT)/partition-table-default.img
  IMX_BUILT_BPTJSON_TARGET := $(PRODUCT_OUT)/partition-table-default.bpt
  
  IMX_INTERNAL_BVBTOOL_MAKE_TABLE_ARGS := \
          --output_gpt $(IMX_BUILT_BPTIMAGE_TARGET) \
          --output_json $(IMX_BUILT_BPTJSON_TARGET) \
          $(foreach file, $(BOARD_BPT_INPUT_FILES), --input $(file))
  
  ifdef BOARD_BPT_DISK_SIZE
  IMX_INTERNAL_BVBTOOL_MAKE_TABLE_ARGS += --disk_size $(BOARD_BPT_DISK_SIZE)
  endif
  
  define imx_build-bptimage-target
    $(call pretty,"Target partition table image: $(IMX_INSTALLED_BPTIMAGE_TARGET)")
    $(hide) $(BPTTOOL) make_table $(IMX_INTERNAL_BVBTOOL_MAKE_TABLE_ARGS) $(BOARD_BPT_MAKE_TABLE_ARGS)
    for addition_partition in $(ADDITION_BPT_PARTITION); do \
      PARTITION_OUT_IMAGE=`echo $$addition_partition | cut -d":" -f1`; \
      PARTITION_INPUT_BPT=`echo $$addition_partition | cut -d":" -f2`; \
      $(BPTTOOL) make_table --output_gpt $(PRODUCT_OUT)/$$PARTITION_OUT_IMAGE.img \
      --output_json $(PRODUCT_OUT)/$$PARTITION_OUT_IMAGE.bpt --input $$PARTITION_INPUT_BPT \
      $(BOARD_BPT_MAKE_TABLE_ARGS); \
     done
  endef
  
  IMX_INSTALLED_BPTIMAGE_TARGET := $(IMX_BUILT_BPTIMAGE_TARGET)
  $(IMX_INSTALLED_BPTIMAGE_TARGET): $(BPTTOOL) $(BOARD_BPT_INPUT_FILES)
          $(imx_build-bptimage-target)
  
  ALL_DEFAULT_INSTALLED_MODULES += $(IMX_INSTALLED_BPTIMAGE_TARGET)
  
  endif # BOARD_BPT_INPUT_FILES
  ```
* `ls -al out/target/product/mek_8q/partition-table*`
  ```
  -rw-rw-r-- 1 wugn wugn  6157 Jul  8 22:50 partition-table-28GB.bpt
  -rw-rw-r-- 1 wugn wugn 34304 Jul  8 22:50 partition-table-28GB.img
  -rw-rw-r-- 1 wugn wugn  6152 Jul  8 22:50 partition-table-7GB.bpt
  -rw-rw-r-- 1 wugn wugn 34304 Jul  8 22:50 partition-table-7GB.img
  -rw-rw-r-- 1 wugn wugn  6156 Jul  8 23:06 partition-table.bpt
  -rw-rw-r-- 1 wugn wugn  6156 Jul  8 22:50 partition-table-default.bpt
  -rw-rw-r-- 1 wugn wugn 34304 Jul  8 22:50 partition-table-default.img
  -rw-rw-r-- 1 wugn wugn 34304 Jul  8 23:06 partition-table.img
  ```
* 分析：`partition-table-28GB:device/fsl/common/partition/device-partitions-28GB-ab-dual-bootloader.bpt`
  ```json
  {
      "settings": {
          "disk_size": "28 GiB",
          "disk_alignment": 2097152,
          "first_partition_offset": 8388608
      },
      "partitions": [
          {
              "ab": true,
              "label": "bootloader",
              "size": "4 MiB",
              "guid": "auto",
              "type_guid": "91af7174-0ecd-483f-8a51-5a46197a4cc3"
          },
          {
              "ab": true,
              "label": "dtbo",
              "size": "4 MiB",
              "guid": "auto",
              "type_guid": "b3c54657-8b47-48bc-9792-37b7c091ae69"
          },
          {
              "ab": true,
              "label": "boot",
              "size": "48 MiB",
              "guid": "auto",
              "type_guid": "bb499290-b57e-49f6-bf41-190386693794"
          },
          {
              "ab": true,
              "label": "system",
              "size": "1792 MiB",
              "guid": "auto",
              "type_guid": "0f2778c4-5cc1-4300-8670-6c88b7e57ed6"
          },
          {
              "label": "misc",
              "size": "4 MiB",
              "guid": "auto",
              "type_guid": "6b2378b0-0fbc-4aa9-a4f6-4d6e17281c47"
          },
          {
              "label": "metadata",
              "size": "2 MiB",
              "guid": "auto",
              "type_guid": "b598858a-5fe3-418e-b8c4-824b41f4adfc"
          },
          {
              "label": "presistdata",
              "size": "1 MiB",
              "guid": "auto",
              "type_guid": "e99d84d7-2c1b-44cf-8c58-effae2dc2558"
          },
          {
              "ab": true,
              "label": "vendor",
              "size": "256 MiB",
              "guid": "auto",
              "type_guid": "2578eab2-a534-43e2-92c0-fed1b01c5d1d"
          },
          {
              "label": "userdata",
              "grow": true,
              "guid": "auto",
              "type_guid": "0bb7e6ed-4424-49c0-9372-7fbab465ab4c"
          },
          {
              "label": "fbmisc",
              "size": "1 MiB",
              "guid": "auto",
              "type_guid": "ebd0a0a2-b9e5-4433-87c0-68b6b72699c7"
          },
          {
              "ab": true,
              "label": "vbmeta",
              "size": "1 MiB",
              "guid": "auto",
              "type_guid": "7d6ea677-fe98-4ffe-9771-f5c3ef6d282c"
          }
      ]
  }
  ```
* 生成文件：`cat out/target/product/mek_8q/partition-table-28GB.bpt`
  ```json
  {
    "settings": {
      "ab_suffixes": ["_a", "_b"],
      "disk_size": 30064771072,
      "disk_alignment": 2097152,
      "disk_guid": "2ee04197-d311-486f-9232-21d6c3c991a9"
    },
    "partitions": [
      {
        "label": "bootloader_a",
        "offset": 8388608,
        "size": 4194304,
        "grow": false,
        "guid": "77974e06-cc8f-47b4-83e0-9710823490d8",
        "type_guid": "91af7174-0ecd-483f-8a51-5a46197a4cc3",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "bootloader_b",
        "offset": 12582912,
        "size": 4194304,
        "grow": false,
        "guid": "c05b79a4-61f6-4925-afbd-fc66bb35b103",
        "type_guid": "91af7174-0ecd-483f-8a51-5a46197a4cc3",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "dtbo_a",
        "offset": 16777216,
        "size": 4194304,
        "grow": false,
        "guid": "459306a0-30c0-419d-adcf-f58f1b6a0571",
        "type_guid": "b3c54657-8b47-48bc-9792-37b7c091ae69",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "dtbo_b",
        "offset": 20971520,
        "size": 4194304,
        "grow": false,
        "guid": "c29de7c8-b8d4-4a86-b4ae-29fcdbb95b60",
        "type_guid": "b3c54657-8b47-48bc-9792-37b7c091ae69",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "boot_a",
        "offset": 25165824,
        "size": 50331648,
        "grow": false,
        "guid": "2b8a6242-5594-429c-8259-cbf9708a72c9",
        "type_guid": "bb499290-b57e-49f6-bf41-190386693794",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "boot_b",
        "offset": 75497472,
        "size": 50331648,
        "grow": false,
        "guid": "b5fbec2b-c7e1-4188-a281-465cdc079773",
        "type_guid": "bb499290-b57e-49f6-bf41-190386693794",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "system_a",
        "offset": 125829120,
        "size": 1879048192,
        "grow": false,
        "guid": "655f82f0-daad-4e73-a006-422236008084",
        "type_guid": "0f2778c4-5cc1-4300-8670-6c88b7e57ed6",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "system_b",
        "offset": 2004877312,
        "size": 1879048192,
        "grow": false,
        "guid": "38d76e89-32b5-437e-9255-3f50ecab2ae9",
        "type_guid": "0f2778c4-5cc1-4300-8670-6c88b7e57ed6",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "misc",
        "offset": 3883925504,
        "size": 4194304,
        "grow": false,
        "guid": "ff0a89b9-7873-4286-8265-860dfd5e589d",
        "type_guid": "6b2378b0-0fbc-4aa9-a4f6-4d6e17281c47",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": false,
        "ab_expanded": false,
        "position": 0
      },
      {
        "label": "metadata",
        "offset": 3888119808,
        "size": 2097152,
        "grow": false,
        "guid": "e785b6c9-264e-479f-a555-d17a225beb79",
        "type_guid": "b598858a-5fe3-418e-b8c4-824b41f4adfc",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": false,
        "ab_expanded": false,
        "position": 0
      },
      {
        "label": "presistdata",
        "offset": 3890216960,
        "size": 1048576,
        "grow": false,
        "guid": "5c0b706d-5ba9-4704-a656-d42401cd4945",
        "type_guid": "e99d84d7-2c1b-44cf-8c58-effae2dc2558",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": false,
        "ab_expanded": false,
        "position": 0
      },
      {
        "label": "vendor_a",
        "offset": 3892314112,
        "size": 268435456,
        "grow": false,
        "guid": "d0f97d37-5d68-4ce5-8d5e-30f7a70cf256",
        "type_guid": "2578eab2-a534-43e2-92c0-fed1b01c5d1d",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "vendor_b",
        "offset": 4160749568,
        "size": 268435456,
        "grow": false,
        "guid": "c8fc0927-38b3-46ea-8170-3fc771292a75",
        "type_guid": "2578eab2-a534-43e2-92c0-fed1b01c5d1d",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "userdata",
        "offset": 4429185024,
        "size": 25627197440,
        "grow": true,
        "guid": "9c351dae-e4a2-412a-8d2a-6602f59e1ed3",
        "type_guid": "0bb7e6ed-4424-49c0-9372-7fbab465ab4c",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": false,
        "ab_expanded": false,
        "position": 0
      },
      {
        "label": "fbmisc",
        "offset": 30056382464,
        "size": 1048576,
        "grow": false,
        "guid": "2764b768-627b-4c2e-8a1b-bc5767a3c88c",
        "type_guid": "ebd0a0a2-b9e5-4433-87c0-68b6b72699c7",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": false,
        "ab_expanded": false,
        "position": 0
      },
      {
        "label": "vbmeta_a",
        "offset": 30058479616,
        "size": 1048576,
        "grow": false,
        "guid": "e97b709e-b3a3-4540-9dc4-cf3de5b6d60e",
        "type_guid": "7d6ea677-fe98-4ffe-9771-f5c3ef6d282c",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      },
      {
        "label": "vbmeta_b",
        "offset": 30060576768,
        "size": 1048576,
        "grow": false,
        "guid": "ebab1887-90f2-474b-8073-a8aa5fa146fb",
        "type_guid": "7d6ea677-fe98-4ffe-9771-f5c3ef6d282c",
        "flags": "0x0000000000000000",
        "ignore": false,
        "ab": true,
        "ab_expanded": true,
        "position": 0
      }
    ]
  }
  ```

## eMMC定制GPT分区

* 《Android_User's_Guide.pdf》：最新版本的只有eMMC的介绍，没有SD卡部分32KB、33KB的介绍  
  ![images/Android_Partition_Info.png](images/Android_Partition_Info.png)
* `cat device-partitions.bpt`
  ```json
  {
      "settings": {
          "disk_size": "28 GiB",
          "disk_alignment": 2097152,
          "first_partition_offset": 8388608
      },
      "partitions": [
          {
              "label": "boot",
              "size": "200 MiB",
              "guid": "auto",
              "type_guid": "C12A7328-F81F-11D2-BA4B-00A0C93EC93B" // EFI(FAT32格式)  https://en.wikipedia.org/wiki/GUID_Partition_Table
          },
          {
              "label": "fs",
              "grow": "true",
              "guid": "auto",
              "type_guid": "linux_fs"
          }
      ]
  }
  ```
* `dpttool.sh`
  ```
  inputFile=device-partitions
  /home/wugn/workspace/imx8qm_auto_ga/imx8_p9.0.0_2.1.0_auto_ga/android9.0.0/out/host/linux-x86/bin/bpttool make_table --output_gpt output/${inputFile}.img --output_json output/${inputFile}.bpt --input ${inputFile}.bpt
  ```
* u-boot: `tmp/deploy/images/imx8qmmek/imx-boot-imx8qmmek-sd.bin`
* kernel、dts: `tmp/work/imx8qmmek-poky-linux/fsl-image-qt5-validation-imx/1.0-r0/boot.img`
* uuu lst file:
  ```
  uuu_version 1.1.29
  #uuu scripts for imx8qm revB board Android imx_pi9.0 sd card
  CFG: FB: -vid 0x1fc9 -pid 0x0129
  SDPS: boot -f uuu-u-boot-imx8qm.imx
  FB: ucmd setenv fastboot_dev mmc
  FB: ucmd setenv mmcdev 1
  FB: ucmd mmc dev 1
  FB: flash bootloader0 u-boot-imx8qm-xen-dom0.imx              # start from 0
  #FB: flash bootloader0 imx-boot-imx8qmmek-sd.bin
  
  FB[-t 600000]: flash gpt device-partitions.img
  
  FB: ucmd mmc partconf 0 0 1 0 
  
  FB: flash boot boot.img
  
  # erase environment variables of uboot
  FB: ucmd mmc erase 0x2000 0x10
  
  FB: done
  ```
* 比较纠结的是为什么eMMC和TF卡都使用同样的烧录方法，注意偏移32k

## bpttool

* https://github.com/ZengjfOS/batTool/tree/bpttool