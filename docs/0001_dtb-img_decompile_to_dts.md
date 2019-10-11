# dtb.img decompile to dts

## 参考文档

* [反编译andriod dtbo.img 为dts](https://blog.csdn.net/movenostop/article/details/95189605)

## 操作 

* `export android=<android root path>`
* `$android/out/host/linux-x86/bin/mkdtimg dump dtbo-imx8qm.img -b dtbo-imx8qm.dtb`
* `dtc -I dtb -O dts dtbo-imx8qm.dtb.0 -o dtbo-imx8qm.dts`

## dtb dts对应关系

* dts iomux源代码
  ```
  /dts-v1/;
  
  / {
      [...省略]
      iomuxc {
          compatible = "fsl,imx8qm-iomuxc";
          pinctrl-names = "default";
          pinctrl-0 = <&pinctrl_hog>;
  
          imx8qm-mek {
  
              pinctrl_hog: hoggrp {
                  fsl,pins = < SC_P_MCLK_OUT0_AUD_ACM_MCLK_OUT0        0x0600004c
                               SC_P_QSPI1A_DATA1_LSIO_GPIO4_IO25       0x0600004c
                              [...省略] >;
              };
              [...省略]
  
          };
      };
      [...省略]
  
  };
  ```
* dtb iomux源代码
  ```
  /dts-v1/;
  
  / {
      [...省略]
      iomuxc {
          compatible = "fsl,imx8qm-iomuxc";
          pinctrl-names = "default";
          pinctrl-0 = <0x12>;
  
          imx8qm-mek {
  
              hoggrp {
                  fsl,pins = <0x73 0x0 0x600004c
                              0xb2 0x3 0x600004c
                              [...省略] >;
                  linux,phandle = <0x12>;
                  phandle = <0x12>;
              };
              [...省略]
  
          };
      };
      [...省略]
  
  };
  ```
* 引脚配置：
  * vendor/nxp-opensource/kernel_imx/include/dt-bindings/pinctrl/pads-imx8qm.h
    ```
    [...省略]
    #define SC_P_MCLK_OUT0                           115  /* AUD.ACM.MCLK_OUT0, AUD.ESAI0.TX_HF_CLK, AUD.ESAI1.TX_HF_CLK, LSIO.GPIO3.IO01 */
    #define SC_P_MCLK_OUT0_AUD_ACM_MCLK_OUT0         SC_P_MCLK_OUT0                     0
    [...省略]
    ```
  * `115` --> `0x73`
* &pinctrl_hog：给一个设备节点添加lable，之后可以通过&lable的形式访问这个lable，这种引用是通过phandle（pointer handle）进行的；