PROCESSING=$(HOME)/processing-3.0.2
CC=gcc
INTROOT=/data/MeteoIntroot
WEBOWNER=meteo:webmaster

##############################################################################################

PY_EXECUTABLES=archiver cronMeteo
PY_PACKAGES=publisherLib
SCRIPTS=startWeather stopWeather
CLIENTS=wsc
EXECUTABLES=child_udp child02 metser061 vsl01

BINDIR=../bin
CLIENTBIN=../clientBin
OBJDIR=../objects
CONFIGDIR=../config
WEBDIR=../meteo
CONFIGROOT=$(INTROOT)/config
LOGSROOT=$(INTROOT)/logs
CLIENTROOT=$(INTROOT)/clients
WEBROOT=$(INTROOT)/meteo

all: .dirs .py_exe .py_pkg .exe .script_exe
	@echo "compile done!"

install: .database .configuration
	@if [ ! -d $(INTROOT) ]; then mkdir $(INTROOT) ; fi ;
	@if [ ! -d $(LOGSROOT) ]; then mkdir $(LOGSROOT) ; fi ;
	@if [ ! -d $(CLIENTROOT) ]; then mkdir $(CLIENTROOT) ; fi ;
	@if [ ! -d $(WEBROOT) ]; then mkdir $(WEBROOT) ; fi ;
	@cp -r $(BINDIR)/* $(INTROOT)/
	@cp -r $(CLIENTBIN)/* $(CLIENTROOT)/
	@cp -r $(WEBDIR)/* $(WEBROOT)/
	@chown -R $(WEBOWNER) $(WEBROOT)/
	@echo "please source file "$(CONFIGROOT)/weather_station_profile
	@echo "install done!"

clients: 
	@rm -rf $(CLIENTBIN)/*
	@$(PROCESSING)/processing-java --sketch=wsc/ --output=$(CLIENTBIN)/linux --force --platform=linux --export
	@cd $(CLIENTBIN); tar czpf linuxwsc.tar.gz linux/
	@echo "all clients built!"

clean:
	@rm -rf $(BINDIR)/*
	@rm -rf $(OBJDIR)/*
	@rm -rf $(INTROOT)/*
	@rm -rf $(CLIENTBIN)/*

.dirs:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(INTROOT)
	@mkdir -p $(CLIENTBIN)
	

.script_exe:
	@for i in $(SCRIPTS); do \
        	cp $${i} $(BINDIR)/$${i}; \
		chmod u+x $(BINDIR)/$${i}; \
        done

.py_exe:
	@for i in $(PY_EXECUTABLES); do \
        	cp $${i}.py $(BINDIR)/$${i}; \
		chmod u+x $(BINDIR)/$${i}; \
        done

.py_pkg:
	@for i in $(PY_PACKAGES); do \
        	cp -r $${i} $(BINDIR)/; \
        done

.exe: 
	@for i in $(EXECUTABLES); do \
        	$(CC) -c $${i}.c -o $(OBJDIR)/$${i}.o; \
		$(CC) $(OBJDIR)/$${i}.o -o $(BINDIR)/$${i}; \
        done


.database:
	@script/createMySQLDB

.configuration:
	@if [ ! -d $(CONFIGROOT) ]; then mkdir $(CONFIGROOT) ; fi ;
	@cp -r $(CONFIGDIR)/* $(CONFIGROOT)/
	@script/createCronJob $(CONFIGROOT)/cronJob.cfg
	@script/createScriptConfiguration $(INTROOT) $(CONFIGROOT)/weather_station_profile
	@script/createDBConfiguration $(CONFIGDIR)/archiver.cfg $(CONFIGROOT)/archiver.cfg
	



