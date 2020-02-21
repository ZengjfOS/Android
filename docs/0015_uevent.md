# uevent

## 示例

ueventd.freescale.rc

```
/dev/block/platform/5b020000\.usdhc/by-name/presistdata 0600   system     system
/dev/block/platform/5b010000\.usdhc/by-name/presistdata 0600   system     system
/dev/block/platform/passthrough/5b020000\.usdhc/by-name/presistdata 0600   system     system
/dev/block/platform/passthrough/5b010000\.usdhc/by-name/presistdata 0600   system     system
/dev/ttyLP1               0660   bluetooth  bluetooth
/dev/ttyLP2               0666   system     gps
/dev/ttyLP3               0666   system     system
/dev/ttyLP4               0666   system     system

[...省略]

/sys/class/rfkill/rfkill0 state 0660 bluetooth bluetooth
```

## 说明

* dev目录下的节点四个字段，sys目录下的节点五个字段
* sys目录下的第一个字段是设备sysfs文件系统目录，这个目录下有uevent文件，第二个字段是文件相对第一个字段的路径，后面的字段意思和dev目录节点一致；
* 正常来说，要使得修改有效，需要重新编译一下Android源代码，这里只需要重新make打包一下就行了；