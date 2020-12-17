# cmd broadcast

用命令发广播

## 参考文档

* [Android 通过adb shell am broadcast发送广播](https://blog.csdn.net/dadaxiaoxiaode/article/details/43231425)

## 蓝牙发现设备广播

* adb shell am broadcast -a android.bluetooth.device.action.FOUND

```
Broadcasting: Intent { act=android.bluetooth.device.action.FOUND flg=0x400000 }
Broadcast completed: result=0
```