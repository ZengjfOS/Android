# ARMv8 ASM

如果想在ARMV8 SoC芯片里学习ARM汇编，那我们该怎么学习呢，自己修改start.S汇编即可，感觉这是最快的修炼之道了。

## 参考文档

* [u-boot移植总结（二）LED点灯调试 和 u-boot加载地址](https://blog.csdn.net/u013236359/article/details/32336183)

## 使用示例

* u-boot start.S
* arch/arm/cpu/armv8/start.S
  ```asm
  ldr x0,=0xbfffff00
  ldr x1,=0x5500
  str x1,[x0]
  ```