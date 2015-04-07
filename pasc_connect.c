#include "pasc.h"
enum CLISTATUS cli_status = UNKNOW;

void request(int conn_fd)
{
	int qid = 100,aid =0;
	int recv_bytes;
	send(conn_fd,&qid,sizeof(int),0);
	if((recv_bytes = recv(conn_fd,&aid,sizeof(int),0)) < 0)
		ERR_EXIT("recv");
	if(recv_bytes == 0||aid != 101)
		cli_status = ENDING;
	else
		cli_status = TALKING;
}

void talking(int conn_fd)
{
	char buf[MAXBUF+1];
	pid_t pid;
	if((pid = fork()) < 0)
		ERR_EXIT("FORK");
	if(pid > 0)   //parent
	{
		int recv_bytes;
		memset(buf,0,sizeof(buf));
		while((recv_bytes = recv(conn_fd,buf,sizeof(buf),0)) > 0)
		{
			printf("peer say: %s\n",buf);
			memset(buf,0,sizeof(buf));
		}
		if(recv_bytes < 0)
			ERR_EXIT("RECV");
		if(recv_bytes == 0)
		{
			shutdown(conn_fd,SHUT_RDWR);
			kill(pid,SIGINT);
		}
	}
	else   //child
	{
		while(1)
		{
			fgets(buf,sizeof(buf),stdin);
			if((send(conn_fd,buf,strlen(buf),0)) < 0)
				ERR_EXIT("SEND");
		}
	}
}

void pasc_connect(char *ipaddr)
{
	int conn_fd;
//	unsigned int cond = 1;
	const unsigned int dest_port = 12341;
	struct sockaddr_in6 dest_addr;
	socklen_t addr_len = sizeof(struct sockaddr_in6);

	if ((conn_fd = socket(PF_INET6,SOCK_STREAM,0)) < 0)
		ERR_EXIT("SOCKET");
	
	memset(&dest_addr,0,addr_len);
	dest_addr.sin6_family = AF_INET6;
	dest_addr.sin6_port = htons(dest_port);
	if((inet_pton(AF_INET6,ipaddr,&dest_addr.sin6_addr)) < 0)
		ERR_EXIT("inet_pton");

	if((connect(conn_fd,(struct sockaddr *)&dest_addr,addr_len)) != 0)
		ERR_EXIT("CONNECT");

	cli_status = REQUEST;

	while(cli_status != ENDING)
	{
		switch(cli_status)
		{
			case UNKNOW:
				printf("cli_status is nuknow!\n");
				exit(1);
				break;
			case REQUEST:
				request(conn_fd);
			break;
			case TALKING:
				talking(conn_fd);
				cli_status = ENDING;
				break;
			case ENDING:
			//	cond = 0;
				break;
			default:
				break;
		}
	}
}
