#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define PORTNUMBER 4001
//#define DEBUG 1
#define DEBUG 0

/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char **argv) {
	int sockfd; /* socket */
	int portno; /* port to listen on */
	int clientlen; /* byte size of client's address */
	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	struct hostent *hostp; /* client host info */
	char buf[BUFSIZE]; /* message buf */
	char *hostaddrp; /* dotted decimal host addr string */
	int optval; /* flag value for setsockopt */
	int n; /* message byte size */

	char host[BUFSIZE], service[BUFSIZE];
	portno=PORTNUMBER;

	/* 
	* socket: create the parent socket 
	*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

  	/* setsockopt: Handy debugging trick that lets 
	   * us rerun the server immediately after we kill it; 
	   * otherwise we have to wait about 20 secs. 
	   * Eliminates "ERROR on binding: Address already in use" error. 
	*/
	optval=1;
	setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int));

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);

	if (bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0) error("ERROR on binding");
	clientlen=sizeof(clientaddr);
  	while (1) {
	    /*
	     * recvfrom: receive a UDP datagram from a client
	     */
		bzero(buf, BUFSIZE);
		n=recvfrom(sockfd,buf,BUFSIZE,0,(struct sockaddr *)&clientaddr,&clientlen);
		if (n<0) error("ERROR in recvfrom");
    		if (DEBUG) { 
		    	if (getnameinfo((struct sockaddr *)&clientaddr,clientlen,host,BUFSIZE,service,BUFSIZE,NI_NUMERICSERV)==0) {
				printf("Received %ld bytes from %s:%s\n",(long)n,host,service);
			}
    	 	}
	 	if (DEBUG) {
	 		if (n==0) printf("remote sensor disconnected!\n");
				hostp=gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	 	}
	 	buf[n]=0;
	 	printf("%s",buf);
    		fflush(stdout);    
  	}
}
