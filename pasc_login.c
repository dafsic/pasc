#include "pasc.h"

enum SRVSTATUS srv_status = WAITING;
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
	printf("child end\n");
	maxconn++;
}

void confirm(int conn_fd)
{
	int qid = 0,aid = 0;
	int recv_bytes = 0;

	if((recv_bytes = recv(conn_fd,&qid,sizeof(int),0)) < 0)
		ERR_EXIT("recv");
	if(recv_bytes == 0||qid != 100)
		srv_status = ENDED;
	else
	{
		aid = 101;
		send(conn_fd,&aid,sizeof(int),0);
		srv_status = TALKED;
	}
}

void deal_conn(int conn_fd)
{
	//int cond = 1;

	while(srv_status != ENDED)
	{
		switch(srv_status)
		{
			case WAITING:
				printf("srv_status is nuknow!\n");
				exit(1);
				break;
			case CONFIRM:
				confirm(conn_fd);
			break;
			case TALKED:
				talking(conn_fd);  //only parent process will return,other exit directry;
				srv_status = ENDED;
				break;
			case ENDED:
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
//			printf("got connection from %s,port %d,socket %d\n",inet_ntop(AF_INET6,&peer_addr.sin6_addr,buf,sizeof(buf)),peer_addr.sin6_port,conn_fd);
			pid = fork();
			if(pid == -1)
				ERR_EXIT("FORK");
			if(pid == 0)
			{
				srv_status = CONFIRM;
				deal_conn(conn_fd);
			}
			else
			{
				continue;
			}
		}
	}
	shutdown(listen_fd,SHUT_RDWR);
}/*}}}*/

