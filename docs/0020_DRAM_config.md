# DRAM config

## u-boot

内存的初始化是在bootloader中进行初始化的

* include/configs/imx8qm_mek.h
  ```
  [...省略]
  #define CONFIG_SYS_SDRAM_BASE           0x80000000
  #define CONFIG_NR_DRAM_BANKS            3                                             // <--------------
  #define PHYS_SDRAM_1                    0x80000000
  #define PHYS_SDRAM_2                    0x200000000
  #define PHYS_SDRAM_1_SIZE               0x80000000      /* 2 GB */
  #define PHYS_SDRAM_2_SIZE               0x40000000      /* 1 GB */
  [...省略]
  ```