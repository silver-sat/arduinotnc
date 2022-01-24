#!/bin/sh

cd /home/pi
echo -n "Up since: "; uptime -s
echo -n "Now: "; date
if [ -f /sys/class/net/eth1/operstate ]; then
  if [ -f .rate.txt ]; then
    RATE=`cat .rate.txt`
    ./tweetpic.py ./overhead.jpg "IP over AX25 and serial via ${RATE}B/s rate-limited TCP"
  else
    ./tweetpic.py ./overhead.jpg "IP over AX25 and serial via rate-limited TCP"
  fi
else
  ./tweetpic.py ./overhead.jpg
fi
echo -n "Up since: "; uptime -s
echo -n "Now: "; date
