# X2 Frame Python SDK
## 简介
本SDK将96Board高通410c板上接收到的视频结构化数据提供通过API提供给Python开发者
## 硬件配置过程
请参考[96Board AWS GGC demo标准配置流程](http://wiki.hobot.cc/pages/viewpage.action?pageId=72456181)
## 使用
### 文件结构
cpp内为SDK代码与示例python程序。
```
cpp
├─ CMakeLists.txt
├─ build
│    ├─ hobotx2.so
│    ├─ smart.py
│    └─ video.py
├─ include
│    └─ ggcdemo
│           ├─ devcom.h
│           ├─ hbipc_ap.h
│           ├─ hbipc_errno.h
│           └─ read_video.h
├─ lib
│    └─ libhbipc_ap.so
├─ sample
│    ├─ CMakeLists.txt
│    ├─ sample.cpp
│    ├─ smart.py
│    └─ video.py
└─ src
       ├─ devcom.cpp
       ├─ hobotx2module.cpp
       └─ read_video.c
```
### 例程
示例主要在build文档中，示例程序使用方法请参考本文档**硬件配置过程**部分。
### 编译
在build文件夹中执行：
```sh
cmake .. && make
```
即可。

### API
#### init_video
hobot_x2.init_video(filename)

Parameters:
- **filename** - name of file in /dev/shm which used for video frame trans.
Returns:
<int>

- `0` if video frame trans initialized successfully.
- `-101` if video frame trans has already been initialized.
- `-11` open file failed.

#### init_smart
hobot_x2.init_smart()

Parameters:
- void
Returns:
<int>

- `0` if smart frame trans initialized successfully.
- `-101` if smart frame trans has already been initialized.
- `-21` connection to HBIPC_AP init failed.
- `-22` provider app start failed.
- `-23` AP <-> CP connection establish failed.

#### deinit_smart
hobot_x2.deinit_smart()

Parameters:
- void
Returns:
<int>

- `0` if smart frame trans deinitialized successfully.
- `-31` smart frame trans was not inited.

#### deinit_video
hobot_x2.deinit_video()

Parameters:
- void
Returns:
<int>

- `0` if video frame trans deinitialized successfully.
- `-31` video frame trans was not inited.

#### hobot_x2.read_video_stream()
hobot_x2.read_video_stream()

Parameters:
- void
Returns:
<tuple> [error code, data]

error code <int>:
- `0` get video frame success.
- `-31` video frame trans was not inited.
- `-32` video frame too large, read error.

data <byte>:
video frame in encoded by protobuf. if error code is nagative value, data will be ''.

#### read_smart_frame
hobot_x2.read_smart_frame()

Parameters:
- void
Returns:
<tuple> [error code, data]

error code <int>:
- `0` get smart frame success.
- `-41` smart frame trans was not inited.
- Other negative value please check [API返回值](http://wiki.hobot.cc/pages/viewpage.action?pageId=53377803)

data <byte>:
smart frame in encoded by protobuf. if error code is nagative value, data will be ''.

#### read_smart_frame_with_id
hobot_x2.read_smart_frame_with_id()

Parameters:
- void
Returns:
<tuple> [error code, frame id, data]

error code <int>:
- `0` get smart frame success.
- `-41` smart frame trans was not inited.
- Other negative value please check [API返回值](http://wiki.hobot.cc/pages/viewpage.action?pageId=53377803)

frame id <int>:
- `-1` process frame id error.
- Other value: frame id

data <byte>:
smart frame in encoded by protobuf. if error code is nagative value, data will be ''.
