#include "pasc.h"


ssize_t sendn(int sockfd,const void*buf,size_t len,int flags)
{
	int n = 0;
	int nleft = len;
	char *bufp = (char *)buf;
	while(nleft > 0)
	{
		if((n = send(sockfd,bufp,nleft,flags)) < 0)
		{
			if(errno == EINTR)
				continue;
			return -1;
		}
		else
		{
			nleft = nleft -n;
			bufp = bufp + n;
		}
	}
	return nleft;
}

ssize_t recvn(int sockfd,void *buf,size_t len,int flags)
{
	int n = 0;
	int nleft = len;
	char *bufp = (char *)buf;
	while(nleft > 0)
	{
		if((n = recv(sockfd,bufp,nleft,flags)) < 0)
		{
			if(errno == EINTR)
				continue;
			return -1;
		}
		if(n == 0)
			break;
		else
		{
			nleft = nleft - n;
			bufp = bufp + n;
		}
	}
	return len - nleft;
}

int recvpmsg(int sockfd,PMSG *msg)
{
	int rbytes;
	int len;
	memset(msg,0,sizeof(PMSG));
	if((rbytes = recvn(sockfd,&len,sizeof(int),0)) < 0)
			return -1;
		if(rbytes < sizeof(int))
			return 0;
		else
		{
			msg->len = ntohl(len);
			if((rbytes = recvn(sockfd,msg->data,msg->len,0)) < 0)
				return -1;
			if(rbytes < msg->len)
				return 0;
			else
				return msg->len;
		}
}
