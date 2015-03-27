/*
 * =====================================================================================
 *
 *       Filename:  pasc_cli.c
 *
 *    Description:  pasc client program 
 *
 *        Version:  1.0
 *        Created:  03/25/2015 09:27:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhaoKaiqiang
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

static int verbose_flag;

void usage();
void pasc_connect(char *ipaddr);
void pasc_query(char *name);
void pasc_join(char *name);
void pasc_login(char *name);
void pasc_exit(char *name);
void pasc_update(char *name);

int main ( int argc, char *argv[] )
{
    int c;
	int option_index = 0;
    static struct option long_options[]=
    {
	{"connect",required_argument,0,'c'},
	{"query",required_argument, 0,'q'},
	{"join",required_argument,0,'j'},
	{"login",required_argument,0,'l'},
	{"exit",required_argument,0,'e'},
	{"update",required_argument,0,'u'},
	{"help",no_argument,0,'h'},
	{0,0,0,0}
    };

    while((c = getopt_long(argc,argv,"hc:q:j:l:e:u:",long_options,&option_index)) != -1)
    {
		switch(c)
			{
			case 'c':
			//if(optarg)
			pasc_connect(optarg);
			break;

			case 'q':
			pasc_query(optarg);
			break;

			case 'j':
			pasc_join(optarg);
			break;
			
			case 'l':
			pasc_login(optarg);
			break;

			case 'e':
			pasc_exit(optarg);
			break;
			
			case 'u':
			pasc_update(optarg);
			break;

			case '?':
			case 'h':
			usage();
			break;

			default:
			printf("default\n");
			abort();
		}
    }

/*    if(optind < argc)
	{
		printf("non-option ARGV-elements:");
		while(optind < argc)
		printf("%s",argv[optind++]);
		printf("\n");
	}*/

    return EXIT_SUCCESS;
}
/* ----------  end of function main  ---------- */

void usage()
{
    printf("mampage is ok!\n");
}

void pasc_connect(char *ipaddr)
{
	printf("ok %s\n",ipaddr);
}
void pasc_query(char *name)
{
	printf("ok %s\n",name);
}
void pasc_join(char *name)
{
	printf("ok %s\n",name);
}
void pasc_login(char *name)
{
	printf("ok %s\n",name);
}
void pasc_exit(char *name)
{
	printf("ok %s\n",name);
}
void pasc_update(char *name)
{
	printf("ok %s\n",name);
}
