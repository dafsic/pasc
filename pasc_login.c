/*
 * =====================================================================================
 *
 *       Filename:  pasc_login.c
 *
 *    Description:  this function is daemon,and listen 441 port.
I*					epoll，防止ddos攻击。
 *
 *        Version:  1.0
 *        Created:  03/29/2015 03:31:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: ZhaoKaiqiang 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int init_daemon(void)
{
	int i,fd;
	switch (fork())    //ensure we are not process group leader,for setsid()
	{
		case -1: 
			return -1;
		case 0:
			printf("child pid %d\n",getpid());
			break;    
		default:
			printf("parent pid %d\n",getpid());
			exit(0);   //parent terminates
//			return 0;
	}

	if (setsid() == -1)
		return -1;

	switch (fork())  //ensure we are not session leader,for forbid open control termianl
	{
		case -1: return -1;
		case 0:  break;
		default: exit(EXIT_SUCCESS);
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

}

void pasc_login(char *name)
{
	int ret = 0;
	ret = init_daemon();
	slee(1000);
	printf("login ok %s\n",name);
}

