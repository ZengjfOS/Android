# Linux DRM modetest And Qt

Android中默认是没有将modetest编译出来的，需要自己额外编译一下，Android.mk是自带了的

## Android.mk

```Makefile
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/Makefile.sources

LOCAL_SRC_FILES := $(MODETEST_FILES)

LOCAL_MODULE := modetest

LOCAL_SHARED_LIBRARIES := libdrm
LOCAL_STATIC_LIBRARIES := libdrm_util

include $(LIBDRM_COMMON_MK)
include $(BUILD_EXECUTABLE)
```

## DRM Test

* https://github.com/grate-driver/libdrm/tree/master/tests
  ```bash
  root@imx8qmmek:/etc# modetest --help
  usage: modetest [-cDdefMPpsCvw]
  
   Query options:
  
          -c      list connectors
          -e      list encoders
          -f      list framebuffers
          -p      list CRTCs and planes (pipes)
  
   Test options:
  
          -P <plane_id>@<crtc_id>:<w>x<h>[+<x>+<y>][*<scale>][@<format>]  set a plane
          -s <connector_id>[,<connector_id>][@<crtc_id>]:<mode>[-<vrefresh>][@<format>]   set a mode
          -C      test hw cursor
          -v      test vsynced page flipping
          -w <obj_id>:<prop_name>:<value> set property
  
   Generic options:
  
          -d      drop master after mode set
          -M module       use the given driver
          -D device       use the given device
  
          Default is to dump all info.
  ```

## 当前屏幕信息

```bash
root@imx8qmmek:/etc# modetest -c
[...省略]
trying to open device 'imx-drm'...done
[  895.762418] i.mx8-hdp 56268000.hdmi: No EDID function, use default video mode
Connectors:
id      encoder status          name            size (mm)       modes   encoders
56      55      connected       HDMI-A-1        0x0             2       55
  modes:
        name refresh (Hz) hdisp hss hse htot vdisp vss vse vtot)
  800x480 60 800 832 848 892 480 484 486 488 26200 flags: phsync, pvsync; type: preferred, driver
  720x480 60 720 736 798 858 480 489 495 525 27000 flags: nhsync, nvsync; type: preferred, driver
  props:
        1 EDID:
                flags: immutable blob
                blobs:

                value:
        2 DPMS:
                flags: enum
                enums: On=0 Standby=1 Suspend=2 Off=3
                value: 0
        5 link-status:
                flags: enum
                enums: Good=0 Bad=1
                value: 0
        7 non-desktop:
                flags: immutable range
                values: 0 1
                value: 0
        6 HDR_SOURCE_METADATA:
                flags: blob
                blobs:

                value:
        57 Content Protection:
                flags: enum
                enums: Undesired=0 Desired=1 Enabled=2
                value: 0
59      58      connected       LVDS-1          295x110         1       58
  modes:
        name refresh (Hz) hdisp hss hse htot vdisp vss vse vtot)
  1920x720 92 1920 1952 1953 1985 720 756 757 793 145600 flags: phsync, pvsync; type: preferred, driver
  props:
        1 EDID:
                flags: immutable blob
                blobs:

                value:
        2 DPMS:
                flags: enum
                enums: On=0 Standby=1 Suspend=2 Off=3
                value: 0
        5 link-status:
                flags: enum
                enums: Good=0 Bad=1
                value: 0
        7 non-desktop:
                flags: immutable range
                values: 0 1
                value: 0

```

## plane info

```bash
root@imx8qmmek:/etc# modetest -p
[...省略]
trying to open device 'imx-drm'...done
[ 2499.441569] i.mx8-hdp 56268000.hdmi: No EDID function, use default video mode
CRTCs:
id      fb      pos     size
32      66      (0,0)   (800x480)
  800x480 60 800 832 848 892 480 484 486 488 26200 flags: phsync, pvsync; type: preferred, driver
  props:
45      67      (0,0)   (1920x720)
  1920x720 92 1920 1952 1953 1985 720 756 757 793 145600 flags: phsync, pvsync; type: preferred, driver
  props:

[...省略]
```

## 基本操作

* 杀死后台进程：/usr/bin/weston
* modetest -s 59@45:1920x720 -v
  ```bash
  root@imx8qmmek:/etc# modetest -s 59@45:1920x720 -v
  [...省略]
  trying to open device 'imx-drm'...done
  [ 3260.153586] i.mx8-hdp 56268000.hdmi: No EDID function, use default video mode
  [ 3260.162699] alloc_contig_range: [bc900, bd100) PFNs busy
  [ 3260.169049] alloc_contig_range: [bca00, bd200) PFNs busy
  [ 3260.175400] alloc_contig_range: [bca00, bd300) PFNs busy
  [ 3260.181696] alloc_contig_range: [bcc00, bd400) PFNs busy
  setting mode 1920x720-92Hz@XR24 on connectors 59, crtc 45
  freq: 93.71Hz
  freq: 92.50Hz
  freq: 92.50Hz
  freq: 92.50Hz
  freq: 92.50Hz
  freq: 92.50Hz
  freq: 92.50Hz
  freq: 92.50Hz
  ```
* modetest -s 56@32:800x480 -v
  ```bash
  root@imx8qmmek:/etc# modetest -s 56@32:800x480 -v
  [...省略]
  trying to open device 'imx-drm'...done
  [ 3296.558518] i.mx8-hdp 56268000.hdmi: No EDID function, use default video mode
  setting mode 800x480-60Hz@XR24 on connectors 56, crtc 32
  [ 3296.578258] [drm] phy_cfg_hdp() num_lanes: 4, mode:800x480p60, color depth: 8-bit, encoding: 1
  [ 3296.587869] [drm] Pixel clock frequency (26200 kHz) is supported in this color depth (8-bit). Settings found in row 0
  [ 3296.599490] [drm] Character clock frequency: 26200 kHz.
  [ 3296.605269] [drm] VCO frequency (2096000 kHz) is supported. Settings found in row 13
  [ 3296.628899] [drm] CDN_API_General_Write_Register_blocking LANES_CONFIG ret = 0
  [ 3296.710235] alloc_contig_range: [bcb00, bcd00) PFNs busy
  [ 3296.716630] alloc_contig_range: [bcc00, bce00) PFNs busy
  freq: 61.16Hz
  freq: 60.19Hz
  freq: 60.19Hz
  freq: 60.19Hz
  freq: 60.19Hz
  ```
* modetest -s 56@32:800x480 -s 59@45:1920x720 -v
  ```C
  // 带双参原理：https://chromium.googlesource.com/chromiumos/third_party/drm/+/master/tests/modetest/modetest.c
  int main(int argc, char **argv)
  {
      [...省略]
      while ((c = getopt(argc, argv, optstr)) != -1) {
          switch (c) {
          [...省略]
          case 's':
              con_args[count].crtc = -1;
              if (sscanf(optarg, "%d:%64s",
                     &con_args[count].id,
                     con_args[count].mode_str) != 2 &&
                  sscanf(optarg, "%d@%d:%64s",
                     &con_args[count].id,
                     &con_args[count].crtc,
                     con_args[count].mode_str) != 3)
                  usage(argv[0]);
              count++;                      
              break;
          [...省略]
          }
          [...省略]
      }
      [...省略]
  }
  ```