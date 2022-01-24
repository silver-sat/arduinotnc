#!/bin/sh
python3 /home/pi/.throttle.py $3 | socat - tcp:${1}:${2},retry=11,interval=5 | python3 /home/pi/.throttle.py $3
