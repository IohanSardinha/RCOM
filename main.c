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
	if(argc < 4)
	{
		fprintf(stderr,"Error: Too little arguments!\n");
		printUsage();
		return -1;
	}
	else if(argc > 4)
	{
		if(strcmp(argv[4],"-d") == OK)
		{
			debug = true;
		}
		else
		{
			fprintf(stderr,"Error: Too many arguments!\n");
			printUsage();
			return -1;
		}	
	}

	if(strcmp(argv[1],"t") == OK || strcmp(argv[1],"T") == OK)
		*role = TRANSMITTER;
	else if(strcmp(argv[1],"r") == OK || strcmp(argv[1],"R") == OK)
		*role = RECIEVER;
	else
	{
		fprintf(stderr,"Error: Wrong role '%s'!\n",argv[1]);
		printUsage();
		return -1;
	}

	if(is_number(argv[2]))
		*port = atoi(argv[2]);
	else
	{
		fprintf(stderr,"Error: Port must be a number\n");
		printUsage();
		return -1;
	}

	strcpy(path, argv[3]);
	if(path[0] == 0)
	{
		fprintf(stderr,"Error: Wrong path\n");
		printUsage();
		return -1;
	}
	return OK;
}

void printUsage()
{
	fprintf(stderr,"Usage:\tmain role[T/R] <port> <path>\n");
	fprintf(stderr,"ex:\tmain t 2 /docs\n");
}