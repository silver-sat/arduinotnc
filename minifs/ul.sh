#!/bin/sh
if [ -f "$1" ]; then
  curl -F "file=@$1" http://192.168.100.101:5001/upload
fi
