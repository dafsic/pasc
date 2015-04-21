#include "pasc.h"

/*
 * 发送定长包处理tcp流协议的粘包问题,头部长度只代表数据的长度
*/

//On success, 0 is return. On error, -1 is return.
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
	return 0;
}

//return recived bytes or return -1 means error.
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

int checkmd5(char src[],char dst[])
{
	int i = 0;
	int ret = 0;
	for(i = 0;i < 16;i++)
	{
		if(src[i] != dst[i])
		{
			ret = -1;
			break;
		}
	}
	return ret;
}

//return 0 means peer break. -1,error. -2 md5 failed.
int recvpmsg(int sockfd,PMSG *msg,char *key)
{
	int rbytes;
	int datalen;
	char mdfive[16];
	memset(msg,0,sizeof(PMSG));
	if((rbytes = recvn(sockfd,&datalen,sizeof(int),0)) < 0)
		return -1;
	if(rbytes < sizeof(int))
		return 0;
	
	if((rbytes = recvn(sockfd,msg->md5,sizeof(char) * 16,0)) < 0)
		return -1;
	if(rbytes < sizeof(char) * 16)
		return 0;

	if(key != NULL)
	{
		decode(&datalen,sizeof(int),key);
		decode(msg->md5,sizeof(char) * 16,key);
	}
	msg->len = ntohl(datalen);
	if((rbytes = recvn(sockfd,msg->data,msg->len,0)) < 0)
		return -1;
	if(rbytes < msg->len)
		return 0;

	if(key != NULL)
	{
		decode(msg->data,msg->len,key);
	}

	//printf("recv data == %s\n",msg->data);
	md5(msg->data,msg->len,mdfive);
	if((checkmd5(msg->md5,mdfive)) == 0)
		return msg->len;
	else
		return -2;
	
}

int sendpmsg(int sockfd,const char *data,char *key)
{
	int sbytes;
	int len,ret;
	PMSG sbuf;
	len = strlen(data);
	//printf("send data == %s\n",data);
	memset(&sbuf,0,sizeof(sbuf));
	strncpy(sbuf.data,data,len);
	sbuf.len = htonl(len);
	md5(sbuf.data,len,sbuf.md5);
	if(key != NULL)
	{
		encode(&(sbuf.len),sizeof(int),key);
		encode(sbuf.md5,sizeof(char) * 16,key);
		encode(sbuf.data,len,key);
	}
	sbytes = len + sizeof(int) + sizeof(char) * 16;
	if((sendn(sockfd,&sbuf,sbytes,0)) < 0)
		ret = -1;
	else
		ret = 0;
	return ret;
}

