#include"player.h"
static char *shortopts = "vhcr:f:";
int long_index;
static struct option long_options[] = 
{
	{"version", no_argument, NULL,'v'},
	{"help", no_argument, NULL, 'h'},
	{"color",no_argument,NULL,'c'},
	{"file", required_argument,NULL,'f'},
	{"resize", required_argument, NULL, 'r'},
	{NULL,0,NULL,0},
};
extern int need_resize;
extern int window;
extern int step;
extern int need_color;
int main(int argc,char* argv[])
{
	char buff[5000000];
	memset(buff, '\0', sizeof(buff));
	setvbuf(stdout, buff, _IOFBF, sizeof(buff));
	int o;
	char filename[50];
	while((o=getopt_long(argc,argv,shortopts,long_options,&long_index))!=-1)
	{
		switch(o)
		{
			case 'v':
				printf("dian-player v1.1\n");
				break;
			case 'h':
				printf("usage: player [--color][-c][--file <filename>][-f <filename>]\n"
						"[--resize <window> <step>][-r <window><step>][--version][-v][--help][-h]\n");
				return 0;
			case 'c':
				need_color = 1;
				break;
			case 'r':
				need_resize = 1;
				if(optind==argc || !atoi(argv[optind-1]) || !atoi(argv[optind]))
				{
					printf("resize failed\n");
					return 0;
				}
				window = atoi(argv[optind-1]);
			        step = atoi(argv[optind]);
				break;
			case 'f':
				strcpy(filename, optarg);
				break;
			case '?':
				printf("unknown option\n");
				printf("usage: player [--color][-c][--file <filename>][-f <filename>]\n"                                                                "[--resize <window> <step>][-r <window><step>][--version][-v][--help][-h]\n");
				return 0;
		}
	}
	init(filename);
	pthread_t pid;
	pthread_create(&pid,NULL,scan,NULL);
	print();
	decoder_close();
	return 0;
}
