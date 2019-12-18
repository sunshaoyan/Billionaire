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
    
