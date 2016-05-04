#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include "sys/ioctl.h"
#define BAUDRATE B19200
#define DEVICE "/dev/ttyAMA0"
//#define DEVICE "/dev/ttyUSB0"

int main()
{
   struct termios oldtio,newtio;
   int i,fd;
   unsigned char ch;
//
// Inizializzazione porta seriale
   fd = open(DEVICE, O_RDWR | O_NOCTTY);
   if (fd < 0 )
   {
     printf("Device %s is not ready\n\n", DEVICE);
     exit(-1);
   }
   tcgetattr(fd,&oldtio); // Salva i precedenti settaggi
   bzero(&newtio, sizeof(newtio));
   newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   newtio.c_iflag = IGNPAR;
   newtio.c_oflag = 0;
   newtio.c_lflag = 0;
   newtio.c_cc[VTIME] = 0;
   newtio.c_cc[VMIN] = 1;
   tcflush(fd, TCIFLUSH);
   tcsetattr(fd,TCSANOW,&newtio);
// Fine inizializzazione porta seriale
//******************************
   while(1)
   {
/*        write(fd,"x",1);*/
        i=read(fd,&ch,1);
        if(i) printf("%c",ch);
        fflush(stdout);
   }
   return(0);
}
