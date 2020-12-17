# Android Studio Settings

在AS中阅读Settings源代码，主要是配置和导入jar包

## Gradle Error

* Android Studio import project
* [Error “Minimum supported Gradle version is 5.1.1. Current version is 4.4.1” after update android studio](https://stackoverflow.com/questions/55782093/error-minimum-supported-gradle-version-is-5-1-1-current-version-is-4-4-1-afte)
  * gradle/wrapper/gradle-wrapper.properties

    ```file
    distributionBase=GRADLE_USER_HOME
    distributionPath=wrapper/dists
    distributionUrl=https\://services.gradle.org/distributions/gradle-5.6.4-bin.zip
    zipStoreBase=GRADLE_USER_HOME
    zipStorePath=wrapper/dists
    ```

  * default: gradle-4.8-bin.zip
* [Andriod Gradle Sync failed](https://stackoverflow.com/questions/60463052/andriod-gradle-sync-failed)
  * build.gradle

    ```file
    // Top-level build file where you can add configuration options common to all sub-projects/modules.
    buildscript {
        repositories {
            mavenCentral()
            google()
            jcenter()
        }
        dependencies {
            classpath 'com.android.tools.build:gradle:3.6.1'
        }
    }
  
    allprojects {
        repositories {
            mavenCentral()
            google()
            jcenter()
        }
    }
    ```

  * 默认只有jcenter()，加上另外两个；
* 安装一下SDK 28；
* 导入系统的jar包：
  * out/soong/.intermediates/frameworks/base/packages/SettingsLib/SettingsLib/android_common/javac/Settings/SettingsLib.jar
  * 将上面的文件拷贝到Android Studio项目的：app/libs
  * [Project Structure] -> [Dependencies] -> [Modules] -> [app] -> [+] -> [jar Dependency] -> [libs]
  * 将AndroidX相关的jar包拷贝出来：
    ```bash
    #!/bin/bash
    
    outdir=jars
    mkdir ${outdir}
    
    jars=(`find * -iname androidx.*.jar`)
    i=0
    
    for jar in "${jars[@]}"
    do
            fileName=`basename $jar`
            echo "${i} copy ${jar} to ${outdir}/${i}-${fileName}"
    
            cp ${jar} ${outdir}/${i}-${fileName}
    
            ((i=i+1))
    done
    
    cp out/target/common/obj/JAVA_LIBRARIES/framework_intermediates/classes.jar ${outdir}
    cp out/soong/.intermediates/frameworks/base/packages/SettingsLib/SettingsLib/android_common/combined/SettingsLib.jar ${outdir}/1-SettingsLib.jar
    cp out/soong/.intermediates/frameworks/base/packages/SettingsLib/SettingsLib/android_common/javac/SettingsLib.jar ${outdir}/2-SettingsLib.jar
    ```
  * 更新libs下的jar包后，要用[Project Structure]同步一下；


## Android Studio继承关系查看

* [Navigate] -> [Type Hierarchy]
* ctrl+h

## 跟代码快捷键

* ctrl + 鼠标左键
* alt + 左右方向键
