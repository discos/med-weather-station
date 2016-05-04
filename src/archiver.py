#!/usr/bin/python

from publisherLib import db
from publisherLib import constants
from publisherLib import configurator
from publisherLib import serverReadout
from publisherLib.logger import logger
import threading
import time;
import datetime


CONST = constants._Const()

class loopControl:
	def __init__(self,conf):
		now=datetime.datetime.utcnow()
		self.tables={"PHT":[now,0],"Wind":[now,0],"Rain":[now,0],"Hail":[now,0]} #last read time, cycle, query format 
		self.columns={"PHT":["ta01","pa01","ua01"],"Wind":["dm01","sm01"],"Rain":["rc01","rd01","ri01","rp01"],"Hail":["hc01","hd01","hi01","hp01"]}  	
		self.updateTime=conf.getUpdateTime()[1]
		self.address=conf.getServerAddress()[1]
		self.port=conf.getServerPort()[1]
		self.dbPassword=conf.getPassword()[1]
		self.tables["PHT"][1]=conf.getPHT()[1]*self.updateTime
		self.tables["Wind"][1]=conf.getWind()[1]*self.updateTime
		self.tables["Rain"][1]=conf.getRain()[1]*self.updateTime
		self.tables["Hail"][1]=conf.getHail()[1]*self.updateTime
		
	def loop(self):
		self.readValues()
		threading.Timer(self.updateTime,self.loop).start()

	def readValues(self):
		server=serverReadout.serverReadout(self.address,self.port)
		if not server.readout():
			logger.logMessage('readValues: '+server.getLastError())
			return
		for tbl in self.tables:
			[lastUpdate,delta]=self.tables[tbl]
			#now=datetime.datetime.utcnow()
			now=server.entryDates[self.columns[tbl][0]]
			d=datetime.timedelta(seconds=delta)
			if lastUpdate+d<now:
				query='insert into Weather.%s values' % (tbl)
				sensors=self.columns[tbl]
				sensorsVal=''
				for sns in sensors:
					sensorsVal=sensorsVal+','+str(server.entryValues[sns])
				logger.logMessage("updated table: "+tbl)
				query=query+'(\''+db.db.dbTime(now)+'\''+sensorsVal+');'
				#logger.logMessage(query)
				self.tables[tbl][0]=now
				self.insertData(query)

	def insertData(self,query):
		data=db.db(self.dbPassword)
		if not data.connect():
			logger.logMessage('database: '+data.getLastError())
			return
		if not data.insert(query):
			logger.logMessage('database: '+data.getLastError())
			return
		data.close()


def main():
	#insertData(13.5,1010.5,65.9)
	updateTime=int(0)
	port=int(0)
	address=''
	pwd=''
	conf=configurator.configurator(CONST.INIFILE)
	logger.logMessage('Reading configuration')
	if not conf.init():
		logger.logMessage('configurator: '+conf.getLastError())
		logger.logMessage('Archiver terminated')
		return
	res,updateTime=conf.getUpdateTime()
	if not res:
		logger.logMessage('configurator: '+conf.getLastError())
		logger.logMessage('Archiver terminated')
		return
	res,address=conf.getServerAddress()
	if not res:
		logger.logMessage('configurator: '+conf.getLastError())
		logger.logMessage('Archiver terminated')
		return
	res,port=conf.getServerPort()
	if not res:
		logger.logMessage('configurator: '+conf.getLastError())
		logger.logMessage('Archiver terminated')
		return
	res,pwd=conf.getPassword()
	if not res:
		logger.logMessage('configurator: '+conf.getLastError())
		logger.logMessage('Archiver terminated')
		return


	logger.logMessage('Weather station server is '+address)
	logger.logMessage('duty cycle is '+str(updateTime)+' seconds')
	loop=loopControl(conf)
	logger.logMessage('Archiver is running from now')
	loop.loop()
	#logger.logMessage('Archiver terminated')
	

if __name__ == '__main__':
    main()
