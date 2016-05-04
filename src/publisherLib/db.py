import MySQLdb

class db:
	def __init__(self,pwd):
		pass
		self.connector=None
		self.password=pwd
		self.lastError=''

	def getLastError(self):
		return self.lastError

	def connect(self):
		result=True
		try:
			self.connector=MySQLdb.connect(host='localhost',user='meteo',passwd=self.password,db='Weather',unix_socket='/data/mysql/mysql.sock')
		except Exception, e:
			self.lastError="connect: " + str(e)
			result=False
		return result

	@staticmethod
	def dbTime(now):
		time='%04d-%02d-%02d %02d:%02d:%02d' %(now.year,now.month,now.day,now.hour,now.minute,now.second)
		return time

	def close(self):
		self.connector.close()
	
	def insert(self,sql):
		result=True
		try:
			cursor=self.connector.cursor()
			cursor.execute(sql)
			self.connector.commit()
		except Exception, e:
			self.connector.rollback()
			self.lastError="insert: "+ str(e)
			result=False
		return result
