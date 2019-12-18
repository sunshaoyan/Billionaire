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
