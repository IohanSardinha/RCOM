#include "main.h"

int main(int argc, char const *argv[])
{
	int role;
	int port;
	char path[255];
	
	if(validateArgs(argc, argv, &role, &port, path) != OK)
		return -1;
	
	printf("%d %d %s\n", role, port, path);

	return 0;
}

int validateArgs(int argc, char const *argv[], int* role, int* port, char* path){
	if(argc < 4)
	{
		printf("Error: Too little arguments!\n");
		printUsage();
		return -1;
	}
	else if(argc > 4)
	{
		printf("Error: Too many arguments!\n");
		printUsage();
		return -1;	
	}

	if(strcmp(argv[1],"t") == OK)
		*role = TRANSMITTER;
	else if(strcmp(argv[1],"r") == OK)
		*role = RECIEVER;
	else
	{
		printf("Error: Wrong role '%s'!\n",argv[1]);
		printUsage();
		return -1;
	}

	if(is_number(argv[2]))
		*port = atoi(argv[2]);
	else
	{
		printf("Error: Port must be a number\n");
		printUsage();
		return -1;
	}

	strcpy(path, argv[3]);
	if(path[0] == 0)
	{
		printf("Error: Wrong path\n");
		printUsage();
		return -1;
	}

	return 0;
}

void printUsage()
{
	printf("Usage:\tmain role[T/R] <port> <path>\n");
	printf("ex:\tmain t 2 /docs\n");
}