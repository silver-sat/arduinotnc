#!/usr/bin/env python3

import time

start = time.time()
start4 = time.time()
print("Initialize...")

import sys, json, os, os.path, datetime

from twitterproxy import Twitter, Twython

from twittercred import *

def tominsec(s):
    return "%d:%02d"%(int(s/60),int(s%60))

httpsproxy = True
doverify = False

if httpsproxy:
    twitter = Twitter(
        consumer_key,
        consumer_secret,
        access_token,
        access_token_secret,
        httpsproxy={'api.twitter.com': '192.168.100.101:8383',
                    'upload.twitter.com': '192.168.100.101:8484'}
    )
else:
    twitter = Twython(
        consumer_key,
        consumer_secret,
        access_token,
        access_token_secret,
    )

print("Elapsed:",tominsec(time.time()-start4))

if doverify:
    start3 = time.time()
    print("Verify login...")
    response = twitter.verify_credentials()
    # print(json.dumps(response,indent=2))
    # print("Response size:",len(response))
    print("Elapsed:",tominsec(time.time()-start3))

if len(sys.argv) > 1:

    start1 = time.time()
    print("Upload photo...")
    photo = open(sys.argv[1], 'rb')
    response = twitter.upload_media(media=photo)
    # print(json.dumps(response,indent=2))
    # print("Response size:",len(response))
    finish1 = time.time()
    print("Elapsed:",tominsec(finish1-start1))
    
    start2 = time.time()
    print("Send status...")
    message = "[%s] Twitter image post, IP over radio! Image size %.0fK, upload time %s, bytes/sec %.0f"%(datetime.datetime.now().ctime(),os.path.getsize(sys.argv[1])/1024,tominsec(finish1-start1),os.path.getsize(sys.argv[1])/(finish1-start1))
    repsonse = twitter.update_status(status=message, media_ids=[response['media_id']])
    # print(json.dumps(response,indent=2))
    # print("Response size:",len(response))
    print("Elapsed:",tominsec(time.time()-start2))

else:

    start2 = time.time()
    print("Send status...")
    message = "[%s] Twitter text post, IP over radio!"%(datetime.datetime.now().ctime())
    repsonse = twitter.update_status(status=message)
    # print(json.dumps(response,indent=2))
    # print("Response size:",len(response))
    print("Elapsed:",tominsec(time.time()-start2))

finish = time.time()
print("Total Elapsed:",tominsec(finish-start))
