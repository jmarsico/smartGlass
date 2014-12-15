from twython import TwythonStreamer, Twython
import argparse
import random
import time
import pprint
import OSC
import socket
import json
import thread

from instagram import client, subscriptions

import urllib
import requests
import os


# twitter params
APP_KEY=<your key>
APP_SECRET=<your app_secret>
OAUTH_TOKEN=<your_token>
OAUTH_TOKEN_SECRET=<yourTokenSecret>

#OSC params
IP_ADDRESS = 'localhost'
PORT = 12345

#Instagram client ID and client secret
CONFIG = {
    'client_id': <your_ID>,
    'client_secret': <your_cliendSecret>,
    'redirect_uri': <your URL>
}


def download_photo(img_url, img_filename):
    try:
        print "current working: " + str(os.getcwd())
        newFileName = str(os.getcwd()) + "/images/" + img_filename
        print newFileName
        f = open(newFileName,'w+')
        f.write(requests.get(img_url).content)
        f.close()
    except:
        print "JM: except"
        return False
    return True



class MyStreamer(TwythonStreamer):

	def on_timeout(self):
		self.disconnect()

	def on_success(self, data):
		global start_mins
		self.c = OSC.OSCClient()
		mins = int(time.time() / 60)
		print mins
		if 'text' in data:
			#print data['text']
			print "tweet"
			self.msg = OSC.OSCMessage(address="/hit")
			#self.msg.append()
			self.c.sendto(msg, (IP_ADDRESS, PORT))
			time.sleep(0.1)
		if mins - start_mins > 1:
			start_mins = mins
			print "mins - start_mins = 0, mins = " + str(mins)
			print "disconnecting"
			self.disconnect()

################ SETUP ###################

#setup twitter stream and twitter api
stream = MyStreamer(APP_KEY, APP_SECRET, OAUTH_TOKEN, OAUTH_TOKEN_SECRET)
twitter = Twython(APP_KEY, APP_SECRET, OAUTH_TOKEN, OAUTH_TOKEN_SECRET)


#set up instagram
api = client.InstagramAPI(**CONFIG)
#get popular images feed
#loc_media = api.media_search(lat="40.4424930", lng="-79.9425530", distance=1000, count=32)


def getInstaPics(_tag):
	tag = str(_tag)
	tag_search, next_tag = api.tag_search(q=tag)
	tag_recent_media, next = api.tag_recent_media(tag_name=tag_search[0].name, count=50)

	photos = []
	counter = 0
	for media in tag_recent_media:
		url = media.get_standard_resolution_url()
		print url
		filename = str(counter) + ".jpg"

		download_photo(url, filename)
		counter = counter + 1



#OSC stuff
c = OSC.OSCClient()
c.print_tracebacks = True
msg = OSC.OSCMessage(address = "/filter")
msg.append(0)
c.sendto(msg, (IP_ADDRESS, PORT))
#c.close()
time.sleep(1)
oldMention=""



#run the stuff
while True:

	#initiate the timer
	start_mins = int(time.time() / 60)
	print "startMins: " + str(start_mins)

	#get newest mention
	fullMsage = twitter.get_mentions_timeline()				#get the full message one time, we will parse this message
	print fullMsage
	
	#get the user
	mentionUser = fullMsage[0]['user']['screen_name']

	#get the text
	mention = fullMsage[0]['text']				
	
	#create the OSC message for /user
	usermsg = OSC.OSCMessage(address = "/user")
	usermsg.append(mentionUser)

	#parse the mention text (remove @news_glass)
	mention = mention.replace("@news_glass ", "")	#"@news_glass"
	mentions = mention.split()
	mention = mentions[0]

	if mention != oldMention:
		print "****** new mention topic ******"
		oldMention = mention
	
	#print results
	print "***********************************************"
	print "********** mention: " + mention + "************"
	print "********** user: " + mentionUser + "***********"

	#create the OSC message for /topic
	thismsg = OSC.OSCMessage(address = "/topic"	)
	thismsg.append(mention)

	#get the photos for that keyword
	thread.start_new_thread(getInstaPics, (mention,))
	
	#send the OSC messages
	c.sendto(thismsg, (IP_ADDRESS, PORT))
	time.sleep(1)
	c.sendto(usermsg, (IP_ADDRESS, PORT))



	
	
	#re-run the stream thread
	stream.statuses.filter(track = mention)


#start new real-time flow
#save images to folder
#oF picks up images 


