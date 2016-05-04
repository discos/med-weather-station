## installazione zdaemon
easy_install zdaemon

## installazione processing
1. https://processing.org/
2. install the udp library inside skecthbook/libraries

## prepare mysql server
1. configure the file /etc/rc.d/init.d/mysqld. An example is provided in folder etc
2. configure the file /etc/my.cnf. An example is provided in foleder etc.

##installazione software 

1. download package
2. set properly variable inside makefile: PROCESSING=$(HOME)/processing-3.0.2
					  CC=gcc
					  INTROOT=/data/MeteoIntroot
					  WEBOWNER=meteo:webmaster
3. make clean all clients install
4. some information requests will be prompted
5. source $INTROOT/config/weather_station_profile in your .bash_profile
6. if the weather station have to start at bootup edit your rc.local file and add the line: su -l meteo -c startWeather

## For web server configuration (Ngix):

1. Add the proper location in file /etc/nginx/nginx.conf. $(INTROOT) should be replaced with the location configured in the makefile

location ~* /meteo {
    autoindex on;
    root   ($INTROOT)/introot;
    index  index.html index.htm;
}

