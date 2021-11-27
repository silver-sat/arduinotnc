#!/bin/sh

cd /home/pi
echo -n "Up since: "; uptime -s
echo -n "Now: "; date
./tweetpic.py ./overhead.jpg
echo -n "Up since: "; uptime -s
echo -n "Now: "; date
