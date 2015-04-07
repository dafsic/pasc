#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <error.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXBUF 1024
#define MAXCONN 5

void usage();
void pasc_connect(char *ipaddr);
void pasc_query(char *name);
void pasc_join(char *name);
void pasc_login(char *name);
void pasc_exit(char *name);
void pasc_update(char *name);
void talking(int conn_fd);

enum CLISTATUS {UNKNOW,REQUEST,TALKING,ENDING};
enum SRVSTATUS {WAITING,CONFIRM,TALKED,ENDED};

#define ERR_EXIT(m)\
	do \
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0);

