import ConfigParser

class configurator:
	def __init__(self,conf):
		self.configFile=conf
		self.configurator=ConfigParser.ConfigParser()
		self.lastError=''

	def init(self):
		try:
			self.configurator.read(self.configFile)
		except Exception, e:
			self.lastError='init: '+str(e)
			return False
		return True	

	def getLastError(self):
		return self.lastError

	def getUpdateTime(self):
		try:
			time=int(self.configurator.get('Archiver','updateTime'))
		except Exception, e:
			self.lastError='getUpdateTime: '+str(e)
			return (False,0)
		return (True,time)

	def getPHT(self):
		try:
			pht=int(self.configurator.get('Archiver','pht'))
		except Exception, e:
			self.lastError='getPHT: '+str(e)
			return (False,0)
		return (True,pht)

	def getHail(self):
		try:
			hail=int(self.configurator.get('Archiver','hail'))
		except Exception, e:
			self.lastError='getHail: '+str(e)
			return (False,0)
		return (True,hail)

	def getWind(self):
		try:
			wind=int(self.configurator.get('Archiver','wind'))
		except Exception, e:
			self.lastError='getWind: '+str(e)
			return (False,0)
		return (True,wind)

	def getRain(self):
		try:
			rain=int(self.configurator.get('Archiver','rain'))
		except Exception, e:
			self.lastError='getRain: '+str(e)
			return (False,0)
		return (True,rain)

	def getServerAddress(self):
		try:
			addr=self.configurator.get('Server','ip')
		except Exception, e:
			self.lastError='getServerAddress: '+str(e)
			return (False,'')
		return (True,addr)
		
	def getServerPort(self):
		try:
			port=int(self.configurator.get('Server','port'))
		except Exception, e:
			self.lastError='getServerPort: '+str(e)
			return (False,0)
		return (True,port)

	def getPassword(self):
		try:
			pwd=self.configurator.get('DataBase','password')
		except Exception, e:
			self.lastError='getPassword: '+str(e)
			return (False,'')
		return (True,pwd)



	

	
