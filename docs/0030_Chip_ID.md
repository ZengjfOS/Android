# Chip ID

## 参考文档

* [android平台获取手机IMSI，IMEI ,序列号，和 手机号的方法](https://www.cnblogs.com/tyjsjl/p/3585051.html)

## Device ID

所以如果你想得到设备的唯一序号， TelephonyManager.getDeviceId() 就足够了。但很明显暴露了DeviceID会使一些用户不满，所以最好把这些id加密了。

```Java
String getuniqueId(){

    TelephonyManager tm = (TelephonyManager) getBaseContext().getSystemService(Context.TELEPHONY_SERVICE);

    String imei = tm.getDeviceId();
    String simSerialNumber = tm.getSimSerialNumber();
    String androidId = android.provider.Settings.Secure.getString(getContentResolver(),android.provider.Settings.Secure.ANDROID_ID);
    UUID deviceUuid =newUUID(androidId.hashCode(), ((long)imei.hashCode() << 32) |simSerialNumber.hashCode());

    String uniqueId= deviceUuid.toString();
    return uniqueId;
}
```

## Chip ID

* cat /sys/devices/soc0/soc_uid
  ```
  100CA80E82899A18
  ```