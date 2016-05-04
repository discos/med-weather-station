#!/usr/bin/python

import socket
import sys
import time
from publisherLib import constants
from publisherLib import configurator

CONST = constants._Const()

Host = ''
Port = 0

sensors = ["dn01", "dm01", "dx01", "sn01", "sm01",
           "sx01", "ta01", "ua01", "pa01", "rc01", "rd01",
           "ri01", "rp01", "hc01", "hd01", "hi01", "hp01"
           ]

sensors_data = []
datafile = "/data/MeteoIntroot/meteo/static/mapData.xml"


client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.setblocking(0)

print CONST.INIFILE

conf=configurator.configurator(CONST.INIFILE)

if not conf.init():
	print "configuration error: "+conf.getLastError()
	sys.exit()
res,Host=conf.getServerAddress()
if not res:
	print "configuration error: "+conf.getLastError()
	sys.exit()
res,Port=conf.getServerPort()
if not res:
	print "configuration error: "+conf.getLastError()
	sys.exit()

print "Attempting to connect to %s on port %s" % (Host, Port)
client.connect((Host, Port))
# time.sleep(0)
# client.settimeout(5)
print "Connected to %s on port %s" % (Host, Port)
for sensor in sensors:
    try:
        client.send("r" + sensor)
        time.sleep(3)
        data = client.recv(4096)
        print "Sensor %s receive %d bytes" % (sensor, len(data))
        sensors_data.append(data)
    except socket.error, e:
        print "Connection to %s on port %s failed: %s" % (Host, Port, e)
        sys.exit()

client.close()
print "Connection closed"

# return sensors_data
str_sensors_data = " \n".join(str(x) for x in sensors_data)

try:
    f = open(datafile, 'w')
    f.write('%s\n' % '<?xml version="1.0" encoding="UTF-8" ?>')
    f.write('%s\n' % '<Sensors>')
    f.write(str_sensors_data)
    f.write('\n%s\n' % '<!-- end -->')
    f.write('\n%s' % '</Sensors>')
    f.close()
    print "File scritto"
except IOError, e:
    print "File error %s" % e
    pass
