# 开发者文档

[TOC]

## 准备开发环境

下载Visual Studio Installer，安装如下组件：

C++的桌面开发（适用于Windows的C++现代应用），勾选

- MSVC 生成工具
- Windows SDK
- Windows C++ CMake工具

设置并记住安装位置

![image-20240903155313231](./../img/VS生成工具)

IDE选择：Visual Studio、vscode、clion均可，我用的是clion。

## 下载源码

```
git clone --recurse-submodules https://github.com/Katock-Cricket/SAAI.Plugin.git
```

进入子仓库pluginSDK，**删除原有的CPed类**(CPed.cpp, CPed.h)，否则编译报错重定义

```
cd SAAI.Plugin
cd thirdparty/plugin-sdk/plugin_sa/game_sa
del CPed.cpp
del CPed.h
```

## 配置项目

CMake配置如下，工具链使用刚刚下载的Visual Studio即MSVC生成工具，架构选择x86，生成器选择Ninja

![image-20240903161035556](./../img/编译配置)

![image-20240903161140845](./../img/工具链)

## 开发内容

（写自己的改进和逻辑）

## 启动编译

目标为saai，点击构建，第一次会先构建pluginSDK的静态库，时间较久。

![image-20240903161419408](./../img/构建目标)

![image-20240903161507475](./../img/构建成功)

## 游戏配置

下载[GTA-SAAI](https://www.123pan.com/s/AErojv-OpImh.html)，游戏预装了SAAI及其全部前置组件，之后如果要测试构建出的saai.asi能不能正常工作，将其复制到GTA-SAAI/scripts，替换原文件，运行游戏即可

## 编译后自动安装asi到游戏

将上文GTA-SAAI的根目录写入环境变量GTA_SA_DIR

![image-20240903161634544](./../img/环境变量)

配置目标为saai的运行选项，可执行文件为copy.bat，内容是将构建出的saai.asi拷贝到GTA-SAAI/scripts

![image-20240903161720934](./../img/运行配置)

之后点击运行即可做到编译-安装一条龙

## 开发SAAI.Server

Server在前述GTA-SAAI/SAAI.Server，使用对应的IDE(vscode, pycharm等，我用的是pycharm)直接打开该文件夹，将解释器设置为./env/python.exe，然后开始开发和改进。