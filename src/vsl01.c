#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/wait.h> 

#define SERVERPORT 5001
#define MAXBUFLEN 1000
#define NCTIME 30
#define NTOK 200
#define DMAN -999.9
#define NSENSSTR 100
#define SLEEPTIME 1

#include "log.i"

char sensid[NTOK][NSENSSTR];
char tok[NTOK][NSENSSTR];

struct _mdata
{
  double datum[NTOK];
  char dtime[NTOK][NCTIME];
  int status[NTOK];
};

struct sockaddr_in their_addr;
struct hostent *he;
socklen_t addr_len;

int str_parsing(int nsens, char *string, struct _mdata *mdata);
int deblank(char *str);
int sockhandling(char *buf,char *buf2,char *serveradd);

int main()
{
   FILE *ptr;
   struct _mdata mdata;
   char dummy[NSENSSTR],serveradd[NSENSSTR];
   char string[MAXBUFLEN];
   char buf1[MAXBUFLEN],buf2[MAXBUFLEN];
   pid_t pid;
   int n,nb,i,nsens;
   int commpipe[2];  
   char *p,descr[NSENSSTR];

   logMessage("Starting server"); 	
	
   if((ptr=fopen("config/vsl01.txt","r"))==NULL)
   {
      logMessage("Can't open configuration file");
      logMessage("Sensor server terminated"); 	
      exit(1);
   }
   fscanf(ptr,"%s",serveradd);
   logMessage("Server address is %s",serveradd);
   fscanf(ptr,"%d\n",&nsens);
   logMessage("Loading %d sensors",nsens);
   for(i=0;i<nsens;i++) { 
      fgets(descr,NSENSSTR,ptr);
      p=strtok(descr,",");
      strcpy(dummy,p);
      n=strlen(dummy);
      deblank(dummy);
      dummy[n]='=';
      dummy[n+1]='\0';
      strcpy(tok[i],dummy);
      p=strtok('\0',"\n");
      strcpy(sensid[i],p);
      deblank(sensid[i]);
      logMessage("sensor %d [%s%s]",i,tok[i],sensid[i]);
   }
   logMessage("All sensors loaded",nsens);
  
   if(pipe(commpipe))
   {
      logMessage("Error in pipe");
      logMessage("Sensor server terminated"); 
      exit(1);
   }
   logMessage("Sensor server ready");
   if( (pid=fork())==-1)
   {
      logMessage("Error while forking the process");
      logMessage("Sensor server terminated");  
      exit(1);        
   }
   if(pid) {
//    Processo padre      
      dup2(commpipe[0],0); 
      close(commpipe[1]);
      setvbuf(stdout,(char*)NULL,_IONBF,0);
      for(i=0;i<nsens;i++)
      {
        mdata.dtime[i][0]='#';
        mdata.dtime[i][1]='\0';
        mdata.datum[i]=DMAN;
        mdata.status[i]=0;
      } 
      while(1) {
        if(fgets(string,MAXBUFLEN,stdin)) {
	  //logMessage(string);
          str_parsing(nsens,string,&mdata);
          string[0]='\0';
        }
        for(i=0;i<nsens;i++) {
          if(mdata.status[i]==1)
          {
            sprintf(buf1,"w %s %f %s",sensid[i],mdata.datum[i],mdata.dtime[i]);
            nb=sockhandling(buf1,buf2,serveradd);
            if(nb!=-1) {
              mdata.status[i]=2;
            }
          }
        }
      }
   }
   else
   {
//    Processo figlio
      dup2(commpipe[1],1);
      close(commpipe[0]);
      if(execl("child_udp","child_udp",NULL)==-1) {
        logMessage("Can't spawn reading process");
	logMessage("Sensor server terminated"); 
        exit(1);
      }
   }
   return(0);
}
//
int str_parsing(int nsens,char *string, struct _mdata *mdata)
{
  int i,j;
  char *p;
  struct tm *gm;
  time_t t;
  char toktype[10],datum[20];
  p=strtok(string,",");
  do{
    p=strtok('\0',",\n");
    if((p)&&(strlen(p)>5))
    {
      strncpy(toktype,p,3);
      toktype[3]='\0';
      for (i=3;i<strlen(p)-1;i++) datum[i-3]=p[i];
      datum[i-3]='\0';
      for(j=0;j<nsens;j++)
      {
        if(!strcmp(toktype,tok[j]))
        {
          t=time(NULL);
          gm=gmtime(&t);
          mdata->datum[j]=atof(datum);
          strftime(mdata->dtime[j],NCTIME,"%Y%m%d%H%M%S",gm);
          mdata->status[j]=1;
        }
      }
    }  
  }while(p);
  return(0);
}

//
int sockhandling(char *buf,char *buf2,char *serveradd)
{
	struct tm *gm;
	time_t t;
	char errtime[NCTIME];
	int sockfd;
	int numbytes,numbytes2;
	t=time(NULL);
	gm=gmtime(&t);
	strftime(errtime,NCTIME,"%Y %m %d %H %M %S",gm);
	if ((he=gethostbyname(serveradd)) == NULL) {
		logMessage("Error in weather station server binding");
                buf2[0]=0;
		sleep(SLEEPTIME);
		return(-1);
	}
	usleep(2500);  
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		logMessage("Error in connecting to weather station server");
                buf2[0]=0;
		close(sockfd);  // 11 sett 2012
		sleep(SLEEPTIME);
		return(-1);
	}
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	their_addr.sin_family = AF_INET;	 // host byte order
	their_addr.sin_port = htons(SERVERPORT); // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
	usleep(2500);
// 	if ((numbytes = sendto(sockfd, buf,strlen(buf),0,(struct sockaddr *)&their_addr, sizeof their_addr))==-1)
	//printf(buf);
	if ((numbytes = sendto(sockfd, buf,strlen(buf),0,(struct sockaddr *)&their_addr, sizeof their_addr)) < 0)
	{
		logMessage("Error in sending data to weather station server");
                buf2[0]=0;
		close(sockfd);   // 11 sett 2012
		sleep(SLEEPTIME);
		return(-1);
	}
 	usleep(2500);
	if ((numbytes2 = recvfrom(sockfd, buf2, MAXBUFLEN-1,0,(struct sockaddr *)&their_addr, &addr_len)) < 0) {
		logMessage("Error in receiving answer from weather station server");
                buf2[0]=0;
		close(sockfd);   // 11 sett 2012
		sleep(SLEEPTIME);
		return(-1);
	}
	//printf(buf2);
        if(numbytes2>=0) buf2[numbytes2]=0;
	close(sockfd);
        return(numbytes2);
}
//
int deblank(char *str)
{
  char *p1,*p;
  p=str;
  p1=str;
  do{
    if(!strchr(" \t",*p))
    {
      *p1=*p;
      p1++;
    }
    p++;
  }while(*p!='\0');
  *p1='\0';
  return(0);
}



