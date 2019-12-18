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
