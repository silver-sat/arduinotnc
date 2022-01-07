#!/bin/sh
python3 /home/pi/.throttle.py $2 | socat - tcp-listen:$1 | python3 /home/pi/.throttle.py $2
