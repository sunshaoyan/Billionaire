# python库定制化开发指南
## 简介
本指南用于需要对Python库C++层做深度定制的开发人员，一般在切换CP端XPP模型或应用后才需要修改C++代码重新编译库。

## 文件结构
cpp内为SDK代码与示例python程序。
```
cpp
├─ CMakeLists.txt
├─ build
├─ include
│    └─ ggcdemo
│           ├─ devcom.h
│           ├─ hbipc_ap.h
│           └─ hbipc_errno.h
├─ lib
│    └─ lib_hbipc_ap.so
├─ sample
│    ├─ smart.py
│    └─ video.py
└─ src
     ├─ devcom.cpp
     └─ hobotx2module.cpp
```

## 编译
在AP侧通过执行
```sh
sudo apt-get install cmake python3-dev
```
安装编译所需程序。

在项目目录执行
```sh
mkdir build & cd build
cmake .. && make
```
编译出`libhobotx2.so`文件，重命名为`hobotx2.so`即可在python中调用。

## 代码细节
程序调用系统软件提供的`lib_hbipc_ap.so`库实现通过spi BIF获取CP端传送的智能帧数据。`hobotx2module.cpp`提供了Python接口的封装（通过Python.h）。
在获取过程中，需要保证`src/devcom.cpp`文件中`UUID_DEFINE`与CP端XPP应用中配置得一致。
（Hackathon中提供的UUID_DEFINE为`0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xe`）。