#include "pasc.h"
//enum CLISTATUS cli_status = UNKNOW;
enum STATUS cli_status = UNKNOW;
char passphrase2[MAXPASS];


void request(int conn_fd)
{
	do
	{
		int seq,pseq;
		int rbytes;
		char data[MAXBUF];
		PMSG rbuf;

		memset(data,0,sizeof(data));
		printf("Type passphrase1:");
		fgets(data,sizeof(data),stdin);
		printf("Type passphrase2:");
		fgets(passphrase2,sizeof(passphrase2),stdin);

		if((sendpmsg(conn_fd,data)) < 0)
			ERR_EXIT("SEND");
		//===========send passphrase1==================================

		srand(time(NULL));
		seq = rand();
		memset(data,0,sizeof(data));
		sprintf(data,"%d",seq);
		if((sendpmsg(conn_fd,data)) < 0)
			ERR_EXIT("SEND");
		//=========send seq=============================================

		if((rbytes = recvpmsg(conn_fd,&rbuf)) == -1)
			ERR_EXIT("recv");
		if(rbytes == -2)
		{
			printf("Data is modfixed illegally!\n");
			cli_status = END;
			break;
		}
		if(rbytes == 0)
		{
			cli_status = END;
			break;
		}
		else
		{
			pseq = atoi(rbuf.data);
			if(pseq != seq + 1)
			{
				cli_status = END;
				break;
			}
			seq = pseq+1;	

			memset(data,0,sizeof(data));
			sprintf(data,"%d",seq);
			if((sendpmsg(conn_fd,data)) < 0)
				ERR_EXIT("SEND");
			cli_status = TALKING;
		}
		
	}while(0);
}

void talking(int conn_fd)
{
	fd_set rfds;
	int retval,rbytes;
	int stdin_fd,maxfd;
	char data[MAXBUF];
	PMSG rbuf;

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
			memset(data,0,sizeof(data));
			fgets(data,sizeof(data),stdin);
			if((sendpmsg(conn_fd,data)) < 0)
				ERR_EXIT("SEND");
		}

		if(FD_ISSET(conn_fd,&rfds))
		{
			//memset(&rbuf,0,sizeof(rbuf));
			rbytes = recvpmsg(conn_fd,&rbuf);
			if(rbytes == -1)
				ERR_EXIT("recvpmsg");
			if(rbytes == -2)
			{
				printf("Data is modfixed illegally!\n");
			}
			else if(rbytes == 0)
			{
				shutdown(conn_fd,SHUT_RDWR);
				break;
			}
			else if(rbytes > 0)
			{
				printf("peer say: %s",rbuf.data);
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
				printf("Connected!\n");
				talking(conn_fd);
				printf("peer end connecting\n");
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
