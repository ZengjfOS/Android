# README

## 参考文档

* [libufdt utils](https://android.googlesource.com/platform/system/libufdt/+/master/utils/)
* [DTB/DTBO Partitions](https://source.android.google.cn/devices/architecture/dto/partitions)


## dtbo.img header

```
00000000: d7b7 ab1e 0001 a568 0000 0020 0000 0020  .......h... ...        // dt_table_header 
00000010: 0000 0001 0000 0020 0000 0800 0000 0000  ....... ........
00000020: 0001 a528 0000 0040 0000 0000 0000 0000  ...(...@........       // dt_table_entry 
00000030: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000040: d00d feed 0001 a528 0000 0038 0001 9760  .......(...8...`       // dtb file
00000050: 0000 0028 0000 0011 0000 0010 0000 0000  ...(............
00000060: 0000 0dc8 0001 9728 0000 0000 0000 0000  .......(........
00000070: 0000 0000 0000 0000 0000 0001 0000 0000  ................
```


## dump dtb and create mutiple dtb for dtbo image

* `./mkdtboimg.py dump dtbo.img -b dtbo`
  ```
  dt_table_header:
                 magic = d7b7ab1e
            total_size = 107880
           header_size = 32
         dt_entry_size = 32
        dt_entry_count = 1
     dt_entries_offset = 32
             page_size = 2048
               version = 0
  dt_table_entry[0]:
               dt_size = 107816
             dt_offset = 64
                    id = 00000000
                   rev = 00000000
             custom[0] = 00000000
             custom[1] = 00000000
             custom[2] = 00000000
             custom[3] = 00000000
  
  ```
* `./mkdtboimg.py create dtbo_tmp.img dtbo.0 dtbo.0 dtbo.0`
* `./mkdtboimg.py dump dtbo_tmp.img -b dtbo`
  ```
  dt_table_header:
                 magic = d7b7ab1e
            total_size = 107944
           header_size = 32
         dt_entry_size = 32
        dt_entry_count = 3
     dt_entries_offset = 32
             page_size = 2048
               version = 0
  dt_table_entry[0]:
               dt_size = 107816
             dt_offset = 128
                    id = 00000000
                   rev = 00000000
             custom[0] = 00000000
             custom[1] = 00000000
             custom[2] = 00000000
             custom[3] = 00000000
  dt_table_entry[1]:
               dt_size = 107816
             dt_offset = 128
                    id = 00000000
                   rev = 00000000
             custom[0] = 00000000
             custom[1] = 00000000
             custom[2] = 00000000
             custom[3] = 00000000
  dt_table_entry[2]:
               dt_size = 107816
             dt_offset = 128
                    id = 00000000
                   rev = 00000000
             custom[0] = 00000000
             custom[1] = 00000000
             custom[2] = 00000000
             custom[3] = 00000000
  ```


## u-boot get dtb from dtbo partion


* `drivers/usb/gadget/f_fastboot.c`
  ```C
  // check dtbo image
  if (be32_to_cpu(dt_img->magic) != DT_TABLE_MAGIC) {
          printf("boota: bad dt table magic %08x\n",
                          be32_to_cpu(dt_img->magic));
          goto fail;
  } else if (!be32_to_cpu(dt_img->dt_entry_count)) {
          printf("boota: no dt entries\n");
          goto fail;
  }
  
  // get dt_table_entry
  struct dt_table_entry *dt_entry;
  // get first dt_table_entry
  dt_entry = (struct dt_table_entry *)((ulong)dt_img +
                  be32_to_cpu(dt_img->dt_entries_offset));
  fdt_size = be32_to_cpu(dt_entry->dt_size);
  memcpy((void *)(ulong)hdr->second_addr, (void *)((ulong)dt_img +
                  be32_to_cpu(dt_entry->dt_offset)), fdt_size);
  ```
* 如上可知，目前u-boot只使用了第一个dtb，如果有需要可以修改这部分代码；


## Android generate dtbo image

* `device/fsl/common/build/dtbo.mk`
  ```
  $(BOARD_PREBUILT_DTBOIMAGE): $(KERNEL_BIN) $(DTS_SRC) | $(MKDTIMG) $(AVBTOOL)
          $(hide) echo "Building $(KERNEL_ARCH) dtbo ..."
          $(hide) PATH=$$PATH $(MAKE) -C $(TARGET_KERNEL_SRC) mrproper
          $(call build_dtb) || exit 1; \
          for dtsplat in $(TARGET_BOARD_DTS_CONFIG); do \
                  DTS_PLATFORM=`echo $$dtsplat | cut -d':' -f1`; \
                  DTB_NAME=`echo $$dtsplat | cut -d':' -f2`; \
                  DTB=`echo $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/arch/$(TARGET_KERNEL_ARCH)/boot/dts/$(DTS_ADDITIONAL_PATH)/$${DTB_NAME}`; \
                  DTBO_IMG=`echo $(PRODUCT_OUT)/dtbo-$${DTS_PLATFORM}.img`; \
                  $(MKDTIMG) create $$DTBO_IMG $$DTB; \
                  $(AVBTOOL) add_hash_footer --image $$DTBO_IMG  \
                          --partition_name dtbo \
                          --partition_size $(BOARD_DTBOIMG_PARTITION_SIZE); \
          done
  
  .PHONY: dtboimage
  dtboimage: $(BOARD_PREBUILT_DTBOIMAGE)
  ```
