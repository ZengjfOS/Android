# SystemServer ServiceManager

## Service Manager

Service Manager和zygote进程奠定了Android的基础。所以从启动顺序上来看，Service Manager和zygote是一个级别的，都是由init启动， 而System Server是由zygote进程fork出来的。init进程通过创建子进程启动这些服务,创建的service都属于 native服务,运行在Linux空间。

servicemanager进程启动 Binder IPC,管理所有的 Android 系统服务。ServiceManager是一个守护进程，负责管理Server并向Client提供查询Server的功能。 内部维护一个list来记录已经注册的所有的service，统一管理，向Client提供查询服务。

## System Server

Init进程启动zygote后， 由zygote启动的第一个java应用，System Server会启动Android的核心服务，如：ActivityManager, WindowManager, PackageManager,etc. SystemServer通过ServiceManager.addService()调用，使用Binder IPC把上面那些核心服务注册到ServiceManager负责维护的Server列表。

System Server 进程的主要功能:
* 加载 android servers 底层函数库
* 启动 android 系统中的 native 服务
* 创建、注册并启动 Android 的系统服务,在独立线程中运行
* 创建 Looper 消息循环,处理 System Server 进程中的事件消息

System Server 创建 Android 系统服务对象,通过Binder注册到 Service Manager 进程。启动过程中最核心的一步是“启动 Android 系统的各种系统服务”,这些系统服务构成了整个 Android frameworks的基础,通过 Binder IPC 为上层应用提供各种功能。