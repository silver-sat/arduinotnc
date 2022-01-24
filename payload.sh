#!/bin/sh

cd /home/pi
echo -n "Up since: "; uptime -s
echo -n "Now: "; date

rm -f auxstart.sh auxstart.log 
./minifs/dl.sh auxstart.sh
if [ -f auxstart.sh ]; then
  sh auxstart.sh > auxstart.log 2>&1
  ./minifs/ul.sh auxstart.log
fi

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
