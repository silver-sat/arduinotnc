#!/bin/env python3

import sys, time, os
limit = int(sys.argv[1])
cursec = None
while True:
    b = os.read(0,1)
    if not b:
        break
    now = time.time()
    nowsec = int(now)
    if cursec != nowsec:
        sent = 0
        cursec = nowsec
    os.write(1,b)
    sent += 1
    if sent >= limit:
        time.sleep(max(cursec+1-time.time(),0))
