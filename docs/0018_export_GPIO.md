# export GPIO

利用Linux GPIO的export调试硬件

SC_P_USDHC2_VSELECT_LSIO_GPIO4_IO10 0x18000020              // rfkill pin

## GPIO分组情况

```
mek_8q:/sys/kernel/debug # cat gpio
gpiochip7: GPIOs 256-287, parent: platform/5d0f0000.gpio, 5d0f0000.gpio:

gpiochip6: GPIOs 288-319, parent: platform/5d0e0000.gpio, 5d0e0000.gpio:

gpiochip5: GPIOs 320-351, parent: platform/5d0d0000.gpio, 5d0d0000.gpio:
 gpio-341 (                    |wp                  ) in  hi
 gpio-342 (                    |cd                  ) in  hi IRQ

gpiochip4: GPIOs 352-383, parent: platform/5d0c0000.gpio, 5d0c0000.gpio:
 gpio-359 (                    |sw-3p3-sd1          ) out lo
 gpio-361 (                    |wifi-enable-gpio    ) out hi
 gpio-362 (                    |BT power enable     ) out hi

gpiochip3: GPIOs 384-415, parent: platform/5d0b0000.gpio, 5d0b0000.gpio:
 gpio-391 (                    |gpio                ) out hi
 gpio-392 (                    |max9286_pwd         ) in  hi IRQ
 gpio-394 (                    |gpio                ) in  hi IRQ

gpiochip2: GPIOs 416-447, parent: platform/5d0a0000.gpio, 5d0a0000.gpio:
 gpio-431 (                    |camera-power-gpios  ) out hi
 gpio-434 (                    |wlreg_on            ) out lo

gpiochip1: GPIOs 448-479, parent: platform/5d090000.gpio, 5d090000.gpio:

gpiochip0: GPIOs 480-511, parent: platform/5d080000.gpio, 5d080000.gpio:
 gpio-499 (                    |charger-enable-gpio ) out hi
```

## 蓝牙节点信息

* `gpio-362 (                    |BT power enable     ) out hi`
* 设备树：
  ```
  bt_rfkill {
     compatible = "fsl,mxc_bt_rfkill";
     bt-power-gpios = <&gpio4 10 GPIO_ACTIVE_LOW>;
     status ="okay";
  };
  ```
* 计算方法：`362 = 352 + 10`

## WiFi节点信息

* `gpio-361 (                    |wifi-enable-gpio    ) out hi`
* 设备树：
  ```
  bcmdhd_wlan@0 {
           compatible = "android,bcmdhd_wlan";
           pinctrl-0 = <&pinctrl_wifi>;
           interrupt-parent = <&gpio3>;
           interrupts = <8 2>;
           gpios = <&gpio3 8 0>;            //oob_irq即wifi_wake_host
           wifi-enable-gpio = <&gpio4 9 1>;
           //wlreg_on-supply = <&wlreg_on>;
  };
  ```
* 计算方法：`361 = 352 + 9`

## GPIO Tool

* https://github.com/ZengjfOS/RaspberryPi/tree/gpioIrq