#include "pasc.h"
char passphrase3[MAXPASS];

//enum SRVSTATUS srv_status = WAITING;
enum STATUS srv_status = UNKNOW;
int maxconn = MAXCONN; 


int init_daemon(void)
{/*{{{*/
	int i,fd;
	switch (fork())    //ensure we are not process group leader,for setsid()
	{
		case -1: 
			return -1;
		case 0:
			break;    
		default:
			exit(0);   //parent terminates
//			return 0;
	}

	if (setsid() == -1)
		return -1;

	switch (fork())  //ensure we are not session leader,for forbid open control termianl
	{
		case -1: return -1;
		case 0: 
			break;
		default: 
			exit(EXIT_SUCCESS);
	}
	
	for(i = 0;i < NOFILE;i++)
		close(i);

	chdir("/");
	umask(0);
	close(STDIN_FILENO);	
	fd = open("/dev/null",O_RDWR); //open return the lowest-numbered file descriptor not currently open for the process.
	if (fd != STDIN_FILENO)
		return -1;
	if(dup2(STDIN_FILENO,STDOUT_FILENO) != STDOUT_FILENO)   //STDIN_FILENO == fd == 0,close stdout and redirect to /dev/null
		return -1;
	if(dup2(STDIN_FILENO,STDERR_FILENO) != STDERR_FILENO)
		return -1;
	return 0;

}/*}}}*/


void handler(int num)
{
	printf("peer end connecting\n");
	maxconn++;
}

void confirm(int conn_fd)
{
	do
	{
		int seq,pseq;
		int rbytes;
		PMSG rbuf;
		char data[MAXBUF];
		memset(data,0,sizeof(data));
//		char stdinbuf[1024];
//		setbuf(stdin,stdinbuf);
		printf("Is connecting......\n");
		if((rbytes = recvpmsg(conn_fd,&rbuf,NULL)) == -1)
			ERR_EXIT("recv");
		if(rbytes == -2)
		{
			printf("Date is modfixed illegally or password error!\n");
			srv_status = END;
			break;
		}
		if(rbytes == 0)
		{
			srv_status = END;
			break;
		}
		else
		{
			printf("peer passphrase1 is %s",rbuf.data);
			printf("Type passphrase2:");
			fflush(stdin);
			//cleanfilebuf(stdin);
		//	memset(stdinbuf,0,1024);
			fgets(passphrase3,sizeof(passphrase3),stdin);
		}
		//========recive passphrase1=========================

		if((rbytes = recvpmsg(conn_fd,&rbuf,passphrase3)) == -1)
			ERR_EXIT("recv");
		if(rbytes == -2)
		{
			printf("Date is modfixed illegally or password error!\n");
			srv_status = END;
			break;
		}
		if(rbytes == 0)
		{
			srv_status = END;
			break;
		}
		else
		{
			//decode(rbuf);
			pseq = atoi(rbuf.data);
			seq = pseq + 1;
			memset(data,0,sizeof(data));
			sprintf(data,"%d",seq);
			if((sendpmsg(conn_fd,data,passphrase3)) < 0)
				ERR_EXIT("SEND");
		}
		//=========recive pseq and send seq====================

		if((rbytes = recvpmsg(conn_fd,&rbuf,passphrase3)) < 0)
			ERR_EXIT("recv");
		if(rbytes == -2)
		{
			printf("3 Date is modfixed illegally or password error!\n");
			srv_status = END;
			break;
		}
		if(rbytes == 0)
		{
			srv_status = END;
			break;
		}
		else
		{
			//decode(rbuf);
			pseq = atoi(rbuf.data);
			if(pseq == seq + 1)
				srv_status = TALKING;
			else
				srv_status = END;
		}

	}while(0);
}

void deal_conn(int conn_fd)
{
	//int cond = 1;
	while(srv_status != END)
	{
		switch(srv_status)
		{
			case UNKNOW:
				printf("srv_status is unknow!\n");
				exit(1);
				break;
			case REQUEST:
				confirm(conn_fd);
			break;
			case TALKING:
				printf("Connected!\n");
				talking(conn_fd,passphrase3);  //only parent process will return,other exit directry;
				srv_status = END;
				break;
			case END:
			//	cond = 0;
				break;
			default:
				break;
		}
	}

	kill(getppid(),SIGUSR1);
	exit(0);
}

void pasc_login(char *name)
{/*{{{*/
	pid_t pid;
	int listen_fd,conn_fd;
	char peeradd[32];
	const unsigned int listen_port = 12341;
	struct sockaddr_in6 serv_addr,peer_addr;
	socklen_t addr_len = sizeof(struct sockaddr_in6);
	
//	if((init_daemon()) == -1)
//		ERR_EXIT("init_daemon");
	
//	sleep(30);
	
	if ((listen_fd = socket(PF_INET6,SOCK_STREAM,0)) < 0)
		ERR_EXIT("SOCKET");

	memset(&serv_addr,0,addr_len);
	serv_addr.sin6_family = AF_INET6;
	serv_addr.sin6_port = htons(listen_port);
	serv_addr.sin6_addr = in6addr_any;

	int reuse = 1;
	setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
	if((bind(listen_fd,(struct sockaddr *)&serv_addr,addr_len)) == -1)
		ERR_EXIT("BIND");

	if((listen(listen_fd,128)) == -1)
		ERR_EXIT("LISTEN");
	signal(SIGUSR1,handler);
	while(maxconn)
	{
		if((conn_fd = accept(listen_fd,(struct sockaddr *)&peer_addr,&addr_len)) == -1)
		{
			ERR_EXIT("ACCEPT");
		}
		else
		{
			maxconn--;
			printf("----->Got a connection from %s, port %d\n",inet_ntop(AF_INET6,&peer_addr.sin6_addr,peeradd,sizeof(peeradd)),peer_addr.sin6_port);
			pid = fork();
			if(pid == -1)
				ERR_EXIT("FORK");
			if(pid == 0)
			{
				close(listen_fd);
				srv_status = REQUEST;
				deal_conn(conn_fd);
			}
			else
			{
				close(conn_fd);
				continue;
			}
		}
	}
	shutdown(listen_fd,SHUT_RDWR);
}/*}}}*/

