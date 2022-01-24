#!/bin/sh
curl -f -s -o "$1" "http://192.168.100.101:5001/download?file=$1" && echo "Success"
