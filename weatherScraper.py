#! /usr/bin/python
import urllib
import urllib2
from urllib import urlopen
import re 
import mechanize
from datetime import datetime
import serial
import sys
#import bluetooth
import time



###############this function is the meat of the program, does all the web work

def scrapePage():
    ip2loc = urlopen('http://www.ip2location.com/').read()
    zipRe = re.compile('<td><label for="chkZIPCode">(\d{5})<')
    
    zipCode = re.findall(zipRe, ip2loc)
    zip = str(zipCode[0])
    
    print "Zip code is "
    print zip
    
    #now get the page for this zipcode
    br = mechanize.Browser()
    br.set_handle_robots(False)
    url = 'http://www.weather.com/'
    br.open(url)
    br.select_form(nr=0)
    br['where'] = zip
    response = br.submit()
    interPage = response.read()
    
    #now manipulate get part of the desired url
    hourlyRe = re.compile('<a class="wx-vnav" href="(.*)US')
    hrByhrLinkList = re.findall(hourlyRe, interPage)
    interPageString = hrByhrLinkList[0]
    
    #and construct the full url
    interPageString += 'US'
    hrByhrURL = "http://www.weather.com/weather/hourbyhour/graph/"
    hrByhrURL += interPageString[19:]
    
    print hrByhrURL
    
    #open the hrByhrURL and get the desired data
    hrByhr = urlopen(hrByhrURL).read()
    
    tempRe = re.compile('<p class="wx-temp">\s*(.*)<sup')
    descriptionRe = re.compile('<p class="wx-phrase">(.*)</p>')
    precipRe = re.compile('<dt>PRECIP:</dt>\n.*<dd>\s*(.*)&')
    windDirRe = re.compile('<dt>WIND:</dt>\n\s*<dd>(.*)a')
    windSpeedRe = re.compile('<dt>WIND:</dt>\n\s*<dd>.*t(.*)m')
    humidityRe = re.compile('<dt>HUMIDITY:</dt>\n.*<dd>\s*(.*)&')
    
    tempList = re.findall(tempRe, hrByhr)
    descriptionList = re.findall(descriptionRe,hrByhr)
    precipList = re.findall(precipRe, hrByhr)
    windDirList = re.findall(windDirRe, hrByhr)
    windSpeedList = re.findall(windSpeedRe, hrByhr)
    humidityList = re.findall(humidityRe, hrByhr)
    
    #now first determine the time to decide how to index into the hrByhr data (try to get close to X hours away by using appropriate 15 min)
    time = str(datetime.now())
    time = int(time[14:16])
    
    hrsAhead = 2 #CHANGE THIS TO CHANGE DESIRED TIME OF FORECAST (can't do more than 4 or less than 1) 
    index = (hrsAhead - 1) * 4
    if time>=15 and time<30:
        index +=1
    elif time>=30 and time <45:
        index +=2
    elif time >=45:
        index +=3
    

    print " index is  " 
    print index
    #they may have take away the 15 min functionalitY!!! or perhaps just late night
    
    print len(windSpeedList)
    windSpeed = str(windSpeedList[index])
    windSpeed = windSpeed[1:]
    
    print "temp is "
    print tempList[index]
    print "cloud cover is "
    print descriptionList[index]
    print "precip % is "
    print precipList[index]
    print "wind direction is "
    print windDirList[index]
    print "wind speed in mph is "
    print windSpeed
    print "humidity is "
    print humidityList[index]

    #return "c:" + str(descriptionList[index]) + ",t:" + str(tempList[index]) + ",p:" + str(precipList[index]) + ",wd:" + str(windDirList[index]) + ",ws:" + str(windSpeed) + ",h:" + str(humidityList[index]) + ";"
    return str(descriptionList[index]) + "," + str(windDirList[index]) + "," + str(precipList[index]) + "," + str(tempList[index]) + "," + str(windSpeed) + "," + str(humidityList[index]) + ";"
####################################################    
    

#this loop calls scrapePage initially then just every 15 min 
def update(ser):
    #while(True):#this should really be while(arduino within range):
    str = scrapePage()
    print("Sending: " + str)
    ser.write(str)
    #time.sleep(900)#change this value if you desire less frequent updates
        
def loop():
    while(True):
        #look for bluetooth
        try:
            #TODO: check if generic
            ser = serial.Serial('/dev/tty.SeeedBTSlave-DevB', 38400)
        except IOError, e:
            #TODO: remove error prints
            print e.errno
            print e
            print("Unable to connect to Weather Station")
            time.sleep(10) # we haven't found it sleep for a bit and try again
            continue 

        # we have found it
        print("Bluetooth connection aquired")
        update(ser)
        time.sleep(10) #should be around 900 or 15 minutes
        

loop() #this kicks the whole thing off!!! laptops on laptops on laptops


#TODO: look into creating daemon
#deal with situation when no network/generally code much more defensively
