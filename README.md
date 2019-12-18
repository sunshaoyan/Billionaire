# Bilionaire - 简单易用的X2芯片Python SDK

![logo](img/logo.jpg)

地平线机器人 Sunrise™ 2 | 96Boards DragonBoard™ 410c

AP Python SDK

Horizon Hackathon 2019 三万块团队荣誉出品

# 简介
本工程提供了在96Boards板上，方便快捷的在Python代码中获取地平线Sunrise™ 2芯片数据并展示的SDK。（人生苦短，我用python）

如下框图，本SDK复用了上文标准配置中的CP端库与AP端驱动，并在上层封装了Python SDK，提供API供调用获取结构化数据。
![structure](img/structure.png)

# 使用方法
本SDK使用的系统配置与环境为2019 Hackathon标准的配置环境。可参考Wiki上的[安装部署手册](http://wiki.hobot.cc/pages/viewpage.action?pageId=74423879)进行配置。

安装完成后，在AP端`git clone`本项目，并将`lib/lib_hbipc_ap.so`复制到系统`/lib`目录下。

此后，按部署教程在CP侧执行`./xpp_start.sh`启动cp侧程序，即可在ap侧执行`sample`文件夹下的示例代码。

**注：本项目中的全部sample均需要在AP上使用sudo命令启动，如：**
```sh
sudo python3 smart.py
```

如需使用本SDK，只需要复制`sample`文件夹中的`hobotx2.so`库到任意位置，在Python代码中import：
```py
import hobotx2   # hobotx2.so
```
即可使用。

示例详细使用方式请参考[Samples](#samples)与[API](#api)部分。

### 自启动脚本
因Hackathon Demo中提供的CP侧程序尚无重连机制，故每次程序重新启动均需要断电重启设备，所以强烈建议在CP侧部署自启动脚本。部署步骤如下：

# Samples
我们在`sample/`目录下提供了编译好的库文件`hobotx2.so`,以及三个示例代码文件，分别展示在三种不同场景下的使用方式。
## 1. 持续循环获取智能数据
#### `sample/smart.py`
在本示例中，我们使用一个无限循环，接收CP发出的智能数据。
```python
#!/usr/bin/python
# -*- coding: UTF-8 -*-
import hobotx2   # hobotx2.so
import base64
import time
import struct

hobotx2.init_smart()
while True:
    err, frame = hobotx2.read_smart_frame()
    if err is not 0:
        print("read smart frame error:", err)
    else:
        print("smart frame:",base64.b64encode(frame))
        print("frame len:",len(frame))
hobotx2.deinit_smart()
```
其中，在程序最开始，我们调用`hobotx2.init_smart()`对SDK进行初始化。
在循环体中，通过`hobotx2.read_smart_frame()`获得智能帧数据（byte形式的protobuf数据）。
该函数为阻塞调用，频率和视频帧率保持一致。

用户可以在循环体中，解析得到的数据并进行相关处理。

## 2. 在单独的线程中获取智能数据
#### `sample/thread_wrapper.py`
在某些场景中，我们需要边获取智能数据，边进行其他非阻塞/耗时操作，例如提供HTTP服务、处理某些硬件通信消息等。
为了实现这一目的，可以通过我们提供的`X2WrapperThread`类，实现在单独的现成中进行智能数据获取与处理功能。
```python
import hobotx2   # hobotx2.so
import threading
import time


class X2WrapperThread(threading.Thread):
    def __init__(self):
        hobotx2.init_smart()
        threading.Thread.__init__(self)
        self.cl = threading.Lock()
        self.example_data = 0
        self.result_data = None

    def run(self):
        while True:
            err, frame = hobotx2.read_smart_frame()
            if err is not 0:
                print("read smart frame error:", err)
            else:
                self.cl.acquire()
                # Do your stuff
                print(self.example_data)
                # Process the frame data
                # self.result_data = DoSomethingWith(frame)
                self.cl.release()

    def set_data(self, data):
        self.cl.acquire()
        # do something else from the main thread
        self.example_data = data
        self.cl.release()

    def get_result(self):
        self.cl.acquire()
        result = self.result_data  # remember to deep copy when necessary
        self.cl.release()
        return result


# a sample test case
if __name__ == '__main__':
    tr = X2WrapperThread()
    tr.start()
    idx = 0
    while True:
        time.sleep(1)
        tr.set_data(idx)
        idx += 1
```
在本示例中，我们将`X2WrapperThread`继承自`threading.Thread`类，在线程`run()`函数中运行读取智能帧数据的循环体。
同时，外界可以通过调用`set_data()`或`get_result()`接口，向内部处理函数传输数据，或在外界获得智能帧处理的结果。

注意此时`get_result()`的频次完全由于外部调用放来控制，不能保证和智能帧获取频率一致。如需精细控制，可自行加工处理。

## 3. 使用WebSocket和前端通信
#### `sample/ws_server.py`
很多情况下我们需要和Web前端进行通信，用于展示或者控制。
我们推荐使用网络通信库`tornado`（安装：`pip3 install tornado`），使用WebSocket协议进行通信。

```python
import hobotx2   # hobotx2.so
import json
import threading
import tornado.web
import tornado.websocket
import tornado.httpserver
import tornado.ioloop
from thread_wrapper import X2WrapperThread

tr = X2WrapperThread()  # use a global variable for communication between threads (you may find a better way to do this)


class WSHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        pass

    def check_origin(self, origin):
        """
        Keep this return true to overcome cross-site problems
        """
        return True

    def on_message(self, msg):
        """
        Communicate with websocket client
        """
        result_data = tr.get_result()  # get from processing thread
        tr.set_data(0)  # set to processing thread
        self.write_message(json.dumps(result_data))  # send data to the client

    def on_close(self):
        pass


class Application(tornado.web.Application):
    def __init__(self):
        handlers = [
                (r'/', WSHandler)
                ]
        settings = {}
        tornado.web.Application.__init__(self, handlers, **settings)


def serve_ws():
    ws_app = Application()
    server = tornado.httpserver.HTTPServer(ws_app)
    server.listen(8080)
    tornado.ioloop.IOLoop.instance().start()


if __name__ == '__main__':
    tr.start()
    print('serving ws')
    serve_ws()
```
此处我们复用了前面的`X2WrapperThread`类对获取智能数据的循环进行封装，使用一个单独的线程运行。
在本例中，我们使用`tornado`实现了一个简单的WebSocket服务，在WS Handler中，通过访问X2WrapperThread的实例与智能数据进行交互。

# API
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
smart frame in encoded by protobuf. if error code is nagative value, data will be `''`.

# 定制化开发
请参考[python库定制化开发指南](http://gitlab.hobot.cc/ptd/experimental/alg/eevee/awesome/bilionaire/tree/master/src)

# 问题反馈与沟通
欢迎同学们在Wiki页面下留言沟通或提出问题。

wiki地址：[Bilionaire 沟通](http://wiki.hobot.cc/pages/viewpage.action?pageId=77975049)

# 团队成员
- 孙韶言
- 张捷
- 刘宇轩
- 李明琦
- 吴庭丞