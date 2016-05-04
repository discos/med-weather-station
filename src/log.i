#include <time.h>
#include <stdarg.h>


void logMessage(char *msg,...); 

void logMessage(char *msg,...) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char *str,*temp;
	va_list	ap;
	str=malloc(256);
	temp=malloc(256);
    	va_start(ap,msg);
	vsnprintf(temp,256,msg,ap);
    	va_end(ap);
	snprintf(str,256,"%04d-%02d-%02d %02d:%02d:%02d ",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
	strcat(str,temp);
	strcat(str,"\n");
	printf(str);
	free(str);
	free(temp);
}
