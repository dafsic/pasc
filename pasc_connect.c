#include "pasc.h"
//enum CLISTATUS cli_status = UNKNOW;
enum STATUS cli_status = UNKNOW;

void request(int conn_fd)
{
	int qid = 100,aid =0;
	int recv_bytes;
	send(conn_fd,&qid,sizeof(int),0);
	if((recv_bytes = recv(conn_fd,&aid,sizeof(int),0)) < 0)
		ERR_EXIT("recv");
	if(recv_bytes == 0||aid != 101)
		cli_status = END;
	else
		cli_status = TALKING;
}

void talking(int conn_fd)
{
	fd_set rfds;
	int retval;
	int recv_bytes;
	int stdin_fd,maxfd;
	char buf[MAXBUF+1];

	FD_ZERO(&rfds);
	stdin_fd = fileno(stdin);
	if(stdin_fd > conn_fd)
		maxfd = stdin_fd+1;
	else
		maxfd = conn_fd+1;
	while(1)
	{
		FD_SET(stdin_fd,&rfds);
		FD_SET(conn_fd,&rfds);
		retval = select(maxfd,&rfds,NULL,NULL,NULL);
		if(retval == -1)
			ERR_EXIT("SELECT");

		if(FD_ISSET(stdin_fd,&rfds))
		{
			memset(buf,0,sizeof(buf));
			fgets(buf,sizeof(buf),stdin);
			if((send(conn_fd,buf,strlen(buf),0)) < 0)
				ERR_EXIT("SEND");
		}
		if(FD_ISSET(conn_fd,&rfds))
		{
			memset(buf,0,sizeof(buf));
			recv_bytes = recv(conn_fd,buf,sizeof(buf),0);
			if(recv_bytes < 0)
				ERR_EXIT("RECV");
			if(recv_bytes == 0)
			{
				shutdown(conn_fd,SHUT_RDWR);
				break;
			}
			if(recv_bytes > 0)
			{
				printf("peer say: %s\n",buf);
				memset(buf,0,sizeof(buf));
			}
		}
	}
}

void pasc_connect(char *ipaddr)
{
	int conn_fd,ret;
//	unsigned int cond = 1;
	const unsigned int dest_port = 12341;
	struct sockaddr_in6 dest_addr;
	socklen_t addr_len = sizeof(struct sockaddr_in6);

	if ((conn_fd = socket(PF_INET6,SOCK_STREAM,0)) < 0)
		ERR_EXIT("SOCKET");
	memset(&dest_addr,0,addr_len);
	dest_addr.sin6_family = AF_INET6;
	//dest_addr.sin6_family = AF_UNIX;
	dest_addr.sin6_port = htons(dest_port);
	if((ret = inet_pton(AF_INET6,ipaddr,&dest_addr.sin6_addr)) <= 0)
	{
		if(ret == 0)
		{
			printf("src does not contain a character stringrepresenting a valid network address in the specified address family\n");
		}
		else
		{
			ERR_EXIT("inet_pton");
		}
	}

	if((connect(conn_fd,(struct sockaddr *)&dest_addr,addr_len)) != 0)
		ERR_EXIT("CONNECT");

	cli_status = REQUEST;

	while(cli_status != END)
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
				cli_status = END;
				break;
			case END:
			//	cond = 0;
				break;
			default:
				break;
		}
	}
}
