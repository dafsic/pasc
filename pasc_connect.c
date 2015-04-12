#include "pasc.h"
//enum CLISTATUS cli_status = UNKNOW;
enum STATUS cli_status = UNKNOW;
char passphrase2[MAXPASS];

void request(int conn_fd)
{
	do
	{
		int seq,pseq;
		int rbytes,sbytes;
		PMSG sbuf,rbuf;
		memset(&sbuf,0,sizeof(sbuf));

		srand(time(NULL));
		seq = rand();
		printf("Type passphrase1:");
		fgets(sbuf.data,sizeof(sbuf.data),stdin);
		printf("Type passphrase2:");
		fgets(passphrase2,sizeof(passphrase2),stdin);

		sbytes = strlen(sbuf.data) + sizeof(int);
		sbuf.len = htonl(strlen(sbuf.data));
		if((sendn(conn_fd,&sbuf,sbytes,0)) < 0)
			ERR_EXIT("SEND");
		//===========send passphrase1==================================

		memset(&sbuf,0,sizeof(PMSG));
		sprintf(sbuf.data,"%d",seq);
		//encode(&seq);
		sbytes = strlen(sbuf.data) + sizeof(int);
		sbuf.len = htonl(strlen(sbuf.data));
		if((sendn(conn_fd,&sbuf,sbytes,0)) < 0)
			ERR_EXIT("SEND");
		//=========send seq=============================================

		if((rbytes = recvpmsg(conn_fd,&rbuf)) < 0)
			ERR_EXIT("recv");
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

			memset(&sbuf,0,sizeof(sbuf));
			sprintf(sbuf.data,"%d",seq);
			sbytes = strlen(sbuf.data) + sizeof(int);
			sbuf.len = htonl(strlen(sbuf.data));
			//encode(sbuf);
			if((sendn(conn_fd,&sbuf,sbytes,0)) < 0)
				ERR_EXIT("SEND");
			cli_status = TALKING;
		}
		
	}while(0);
}

void talking(int conn_fd)
{
	fd_set rfds;
	int retval;
	int rbytes,sbytes;
	int stdin_fd,maxfd;
	PMSG sbuf,rbuf;

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
			memset(&sbuf,0,sizeof(sbuf));
			fgets(sbuf.data,sizeof(sbuf.data),stdin);
			sbytes = strlen(sbuf.data) + sizeof(int);
			sbuf.len = htonl(strlen(sbuf.data));
			if((sendn(conn_fd,&sbuf,sbytes,0)) < 0)
				ERR_EXIT("SEND");
		}
		if(FD_ISSET(conn_fd,&rfds))
		{
			//memset(&rbuf,0,sizeof(rbuf));
			rbytes = recvpmsg(conn_fd,&rbuf);
			if(rbytes < 0)
				ERR_EXIT("recvpmsg");
			if(rbytes == 0)
			{
				shutdown(conn_fd,SHUT_RDWR);
				break;
			}
			if(rbytes > 0)
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
