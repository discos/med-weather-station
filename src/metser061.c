/*
Franco Buffa OAC
ver. 03/10/2012
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define MYPORT 5001
#define MAXBUFLEN 1000
#define DMAN 0.0
#define NTOK 200
#define NCTIME 30
#define NSENSSTR 100
#define STOPCODE "83445"
#define VERSION "0.6.1 (Nov, 2015)"

#include "log.i"

char sensid[NTOK][NSENSSTR];  // identificatore sensore, p.e. th01
char info[NTOK][NSENSSTR];    // info sensore p.e. heating temp

struct _mdata
{
  double datum[NTOK];
  char dtime[NTOK][NCTIME];
};

int sockhandling(int nsens, struct _mdata *mdata);
int xmlout(char *bufo,char *dtime,char *sensid,char *info, double datum);
void formatCurrentTime(char *output);

int main(int argc, char *argv[])
{
   FILE *ptr;
   int nsens,i;
   struct _mdata mdata;
   char *p,descr[NSENSSTR];
   char currentTime[32];
   if(argc > 1)
   {
     if(!strcmp(argv[1],"-v")) {
       printf("Version: %s\n",VERSION);
       exit(1);
     }    
   } 
   logMessage("Starting weather server version %s",VERSION);		
   if((ptr=fopen("config/metser.txt","r"))==NULL)
   {
      logMessage("Can't open configuration file");
      logMessage("Weather server terminated");
      exit(1);
   }
   fgets(descr,NSENSSTR,ptr);
   nsens=atoi(descr);
   logMessage("Loading %d sensors",nsens);
   formatCurrentTime(currentTime);
   for(i=0;i<nsens;i++)
   { 
      fgets(descr,NSENSSTR,ptr);
      p=strtok(descr,",");
      strcpy(sensid[i],p);
      p=strtok('\0',"\n");
      strcpy(info[i],p);
      logMessage("sensor %d [%s][%s]",i,sensid[i],info[i]);
      strcpy(mdata.dtime[i],currentTime);
      mdata.datum[i]=DMAN;
   }
//
   logMessage("Weather server ready"); 	
   if(sockhandling(nsens,&mdata)) exit(1);
//
   return 0;
}

int sockhandling(int nsens, struct _mdata *mdata)
{
	char sid[100],cmd,epoc[NCTIME], val[100];
	int i;
	int sockfd,flag=0;
	struct sockaddr_in my_addr;    // my address information
	struct sockaddr_in their_addr; // connector's address information
	socklen_t addr_len;
	int numbytes;
	char buf[MAXBUFLEN],bufo[MAXBUFLEN];
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      	    logMessage("Can't open socket");
            logMessage("Weather server terminated");
            exit(1);
	}
	my_addr.sin_family = AF_INET;         // host byte order
	my_addr.sin_port = htons(MYPORT);     // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(my_addr.sin_zero,'\0', sizeof my_addr.sin_zero);
	if (bind(sockfd,(struct sockaddr *)&my_addr,sizeof my_addr)==-1) {
		logMessage("Can't bind socket");
		logMessage("Weather server terminated");
		exit(1);
	}
	addr_len = sizeof their_addr;
	while(!flag) {
		if ((numbytes=recvfrom(sockfd,buf,MAXBUFLEN-1,0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			logMessage("Error reading data from socket");
			logMessage("Weather server terminated");
			flag=1;
		}
		else {
			buf[numbytes]='\0';
			//printf("received \"%s\" from [%s]\n",buf,inet_ntoa(their_addr.sin_addr));
			if(buf[0]=='w') {
      				val[0]='\0';
				epoc[0]='\0';
				sid[0]='\0';
				sscanf(buf,"%c %s %s %s",&cmd,sid,val,epoc);
				if (!strcmp(epoc,"*")) {
				        time_t t;
					struct tm *tmp;
					const char* fmt = "%Y%m%d%H%M%S";
					t = time(NULL);
					tmp = gmtime(&t);
					strftime(epoc, sizeof(epoc),fmt,tmp);
				}
				if(strlen(val)&&strlen(epoc)) {
					for(i=0;i<nsens;i++) {
						if(!strcmp(sensid[i],sid)) {
							mdata->datum[i]=atof(val);
							strcpy(mdata->dtime[i],epoc);
							xmlout(bufo,mdata->dtime[i],sensid[i],info[i],mdata->datum[i]);
						        break;
         					}
       					}
					if (i>=nsens) {
						xmlout(bufo,"error","sensor not found","error",1.0);
						logMessage("request for unknown sensor %s",sid);	
					}
      				}
     			}
     			else if (buf[0]=='r') {
				sid[0]='\0';
				sscanf(buf,"%c %s",&cmd,sid);
      				for(i=0;i<nsens;i++) {
        				if(!strcmp(sensid[i],sid)) {
						xmlout(bufo,mdata->dtime[i],sensid[i],info[i],mdata->datum[i]);
						break;
					}
      				}
				if (i>=nsens) {
					xmlout(bufo,"error","sensor not found","error",1.0);
					logMessage("request for unknown sensor %s",sid);
				}
     			}
     			else if(buf[0]=='s') {
				sid[0]='\0';
				sscanf(buf,"%c %s",&cmd,sid);
				if(!strcmp(STOPCODE,sid)) {
					xmlout(bufo,"stop","quit","stop",0.0);
					logMessage("Stop issued from [%s]",inet_ntoa(their_addr.sin_addr));
					flag=1;
				}
				else {
					xmlout(bufo,"error","wrong stop code","error",3.0);
					logMessage("stop issued but the code is not correct from [%s]",buf,inet_ntoa(their_addr.sin_addr));
				}
      			}
      			else {
			         xmlout(bufo,"error","unknown command","error",2.0);
				 logMessage("wrong command request \'%s\', from [%s]",buf,inet_ntoa(their_addr.sin_addr));	
			}
     		}
		//printf("mando %s\n",bufo);
		if ((numbytes=sendto(sockfd,bufo,strlen(bufo),0,(struct sockaddr *)&their_addr,sizeof their_addr))==-1) {
			logMessage("Error writing data to socket");
			logMessage("Weather server terminated");
	       		flag=1;
     		}
	}
     	close(sockfd);
	return(flag);
}

void formatCurrentTime(char *output)
{
    time_t rawtime;
    struct tm *timeinfo;
    time (&rawtime);
    timeinfo=gmtime(&rawtime);
    output[0]='\0';
    sprintf(output,"%04d%02d%02d%02d%02d%02d",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
}

int xmlout(char *bufo,char *dtime,char *sensid,char *info, double datum)
{
  sprintf(bufo,"<Sensor><Id>%s</Id><Val>%f</Val><Date>%s</Date><Info>%s</Info></Sensor>",sensid,datum,dtime,info);
  //logMessage(bufo);
  return(0);
}



