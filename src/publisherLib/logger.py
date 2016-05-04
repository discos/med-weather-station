import time

class logger:
	@staticmethod
	def logMessage(message):
		lt=time.localtime(time.time())
		timeStamp='%04d-%02d-%02d %02d:%02d:%02d' % (lt.tm_year,lt.tm_mon,lt.tm_mday,lt.tm_hour,lt.tm_min,lt.tm_sec)
		print timeStamp+' '+message
