#include "pasc.h"
enum STATUS status = NUKNOW;

void request(int conn_fd)
{
	int qid = 100,aid =0;
	int recv_bytes;
	send(conn_fd,&id,sizeof(int),0);
	if((recv_bytes = recv(conn_fd,&aid,sizeof(int),0)) < 0)
		ERR_EXIT("recv");
	if(recv_bytes = 0)
		status = END;
	else if(aid != 200)
		status = END;
	else
		status = TALKING;
}

void talking(int conn_fd)
{
	printf("talking ok!\n");
}

void pasc_connect(char *ipaddr)
{
	pid_t pid;
	int conn_fd;
	bool cond = true;
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

	while(cond)
	{
		switch(status)
		{
			case UNKNOW:
				status = REQUEST;
				break;
			case REQUEST:
				request(conn_fd);
			break;
			case TALKING:
				talking(conn_fd);
				break;
			case END:
				cond = false;
				break;
			default:
				break;
		}
	}
	sleep(30);
	shutdown(conn_fd,SHUT_RDWR);
}
