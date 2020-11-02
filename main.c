#include "main.h"

int main(int argc, char const *argv[])
{
	int role, port, fd;
	char path[255];
	
	if(validateArgs(argc, argv, &role, &port, path) != OK)
		return -1;

	if((fd = llopen(port,role)) < 0)
	{
		fprintf(stderr, "Error: Could not stablish connection!\n");
		return -1;
	}
	printf("Connection stablised!\n");

	switch(role)
	{
		case TRANSMITTER:
			if(transmitterMain(fd, path) < 0)
				return -1;
			break;

		case RECIEVER:
			if(recieverMain(fd, path) < 0)
				return -1;
			break;
	}

	if(llclose(fd) < 0)
	{
		fprintf(stderr, "Error: Could not disconnect properly!\n");	
		return -1;
	}

	printf("Successfully disconnected!\n");

	return 0;
}

int validateArgs(int argc, char const *argv[], int* role, int* port, char* path)
{
	int optionals = 0;
	if(argc < 4)
	{
		fprintf(stderr,"Error: Too few arguments!\n");
		printUsage();
		return -1;
	}
	else if(argc > 9)
	{
		fprintf(stderr,"Error: Too many arguments!\n");
		printUsage();
		return -1;
	}

	for(int i = 1; i < argc; i++)
	{
			if(strcmp(argv[i],"-t") == OK || strcmp(argv[i],"-T") == OK)
				*role = TRANSMITTER;
			else if(strcmp(argv[i],"-r") == OK || strcmp(argv[i],"-R") == OK)
				*role = RECIEVER;

			else if(strcmp(argv[i],"-p") == OK)
			{
				if(is_number(argv[i+1]))
				{
					*port = atoi(argv[i+1]);
					i++;
				}
				else
				{
					fprintf(stderr,"Error: Port must be a number\n");
					printUsage();
					return -1;
				}
			}

			else if(strcmp(argv[i],"-f") == OK)
			{
				strcpy(path, argv[i+1]);
				if(path[0] == 0)
				{
					fprintf(stderr,"Error: Bad path\n");
					printUsage();
					return -1;
				}
				i++;
			}

			else if(strcmp(argv[i],"-d") == OK)
			{
				debug = true;
				optionals++;
			}

			else if(strncmp(argv[i],"--send-time-out=",16) == OK)
			{
				if(is_number(argv[i]+16))
				{
					send_time_out = atoi(argv[i]+16);
					optionals++;
				}
				else
				{
					fprintf(stderr,"Error: Send time out must be a number\n");
					printUsage();
					return -1;
				}
			}

			else if(strncmp(argv[i],"--read-time-out=",16) == OK)
			{
				if(is_number(argv[i]+16))
				{
					read_time_out = atoi(argv[i]+16);
					optionals++;
				}
				else
				{
					fprintf(stderr,"Error: Read time out must be a number\n");
					printUsage();
					return -1;
				}
			}

			else if(strncmp(argv[i],"--send-tries=",13) == OK)
			{
				if(is_number(argv[i]+13))
				{
					send_tries = atoi(argv[i]+13);
					optionals++;
				}
				else
				{
					fprintf(stderr,"Error: Send time out must be a number\n");
					printUsage();
					return -1;
				}
			}
			else
			{
				fprintf(stderr,"Error: Unknown argument '%s'\n", argv[i]);
				printUsage();
				return -1;
			}
	}

	if(argc > 6 + optionals)
	{
		fprintf(stderr,"Error: Too many arguments!\n");
		printUsage();
		return -1;
	}

	return OK;
}

void printUsage()
{
	fprintf(stderr,"Usage:\tmain -role[t/r] -p <port> -f <path>\noptional: --send-time-out=[int] --read-time-out=[int] --send-tries=[int] -d(Debug)\n");
	fprintf(stderr,"ex:\tmain -t -p 2 -f pinguim.gif\n");
}