#include "pasc.h"


int main ( int argc, char *argv[] )
{

	char opt;
    if (argc <= 1 || argc >= 4)
	{
		usage();
		return 1;
	}
	
    opt = getopt(argc,argv,"hc:q:j:l:e:u:");
	switch(opt)
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

		case 'h':
		case '?':

		default:
			usage();
			break;
		}	

/* 
    if(optind < argc)
	{
		printf("non-option ARGV-elements:");
		while(optind < argc)
		printf("%s",argv[optind++]);
		printf("\n");
	} */

    return EXIT_SUCCESS;
}
/* ----------  end of function main  ---------- */

void usage()
{
    printf("Usage:\n");
	printf("	pasc [-ehjqu] [-c ipv6_addr] [-l name]\n");
}

void pasc_query(char *name)
{
	printf("ok %s\n",name);
}
void pasc_join(char *name)
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
