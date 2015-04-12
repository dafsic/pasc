#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXBUF 1024
#define MAXCONN 5
#define MAXPASS 256

enum STATUS {UNKNOW,REQUEST,TALKING,END};

typedef struct pasc_msg{
	int len;
	char data[MAXBUF];
}PMSG;


void usage();
void pasc_connect(char *ipaddr);
void pasc_query(char *name);
void pasc_join(char *name);
void pasc_login(char *name);
void pasc_exit(char *name);
void pasc_update(char *name);
void talking(int conn_fd);
ssize_t sendn(int sockfd,const void*buf,size_t len,int flags);
ssize_t recvn(int sockfd,void *buf,size_t len,int flags);
int recvpmsg(int sockfd,PMSG *msg);



//enum CLISTATUS {UNKNOW,REQUEST,TALKING,ENDING};
//enum SRVSTATUS {WAITING,CONFIRM,TALKED,ENDED};
#define ERR_EXIT(m)\
	do \
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0);

