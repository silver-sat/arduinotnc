#!/usr/bin/env python3

import sys, json, time, os, os.path, datetime
from twython import Twython

# Known as API key on twitter website
consumer_key        = ''
# Known as API secret key on twitter website
consumer_secret     = ''
access_token        = ''
access_token_secret = ''

client_args = {
    # 'verify': False,
    # 'stream': True,
}

twitter = Twython(
    consumer_key,
    consumer_secret,
    access_token,
    access_token_secret,
    client_args=client_args
)

def tominsec(s):
    return "%d:%02d"%(int(s/60),int(s%60))

start = time.time()
# start3 = time.time()
# print("Verify login...")
# response = twitter.verify_credentials()
# print(json.dumps(response,indent=2))
# print("Response size:",len(response))
# print("Elapsed:",tominsec(time.time()-start3))
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
finish = time.time()
print("Total Elapsed:",tominsec(finish-start))
