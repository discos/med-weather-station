from socket import *
import sys
from xml.dom.minidom import parseString
import time
import os
import datetime

#dm01,wind dir ave [deg]
#sm01,wind speed ave [m/s]
#ta01,air temp [°C]
#ua01,rel. humidity [%]
#pa01,air pressure [hPa]
#rc01,rain amount [mm]
#rd01,rain duration [s]
#ri01,rain intensity [mm/h]
#rp01,rain peak duration [s]
#hc01,hail amount [hits/cm2]
#hd01,hail duration [s]
#hi01,hail intensity [hits/cm2h]
#hp01,hail peak duration [s]


class serverReadout:
	def __init__(self,ip,port):
		self.serverip=ip
		self.serverport=port
		self.lastError=''
		self.client=None
		self.sensors=["dm01","sm01","ta01","ua01","pa01","rc01","rd01","ri01","rp01","hc01","hd01","hi01","hp01"]
		self.names={"dm01": "wind_dir_ave","sm01":"wind_speed_ave","ta01":"temperature","ua01":"humidity","pa01":"pressure","rc01":"rain_ammount",
			    "rd01": "rain_duration","ri01": "rain_intensity","rp01":"rain_peak_duration","hc01":"hail_ammount","hd01":"hail_duration",
			    "hi01": "hail_intensity", "hp01":"hail_peak_duration"}
		self.entryDates={}
		self.entryValues={}
		self.entryNames={}

	def close(self):
		self.client.close()
		self.client=None

	def getLastError(self):
		return self.lastError

	def readout(self):
		try:
			self.client=socket(AF_INET,SOCK_DGRAM)
			self.client.connect((self.serverip,self.serverport))
			self.client.settimeout(4)
		except Exception, e:
			self.lastError='readout: open: '+str(e)
			self.close()
			return False
		self.entryDates={}
		self.entryValues={}
		self.entryNames={}
		for sns in self.sensors:
			try:
    				self.client.send("r "+sns)
		 		data=self.client.recv(1024)
				dom=parseString(data)
				#print data
				xmlTag=dom.getElementsByTagName('Date')[0].toxml()
    				date=xmlTag.replace('<Date>','').replace('</Date>','')
				xmlTag=dom.getElementsByTagName('Val')[0].toxml()
				value=xmlTag.replace('<Val>','').replace('</Val>','')
				if date>0.0:
					self.entryDates[sns]=self.decodeTime(date)
					self.entryValues[sns]=value
					self.entryNames[sns]=self.names[sns]
			except Exception, e:
				self.lastError='readout: inputpoutput '+sns+': '+str(e)
				self.close()
				return False
		self.close()
		return True

	def decodeTime(self,timeStamp):
		year=int(timeStamp[0:4])
		month=int(timeStamp[4:6])
		day=int(timeStamp[6:8])
		hours=int(timeStamp[8:10])
		mins=int(timeStamp[10:12])
		secs=int(timeStamp[12:14])
		d=datetime.datetime(year,month,day,hours,mins,secs)
		return d
