#include "transmitter.h"

int transmitterMain(int fd, char* path)
{
	int file_fd;
	struct stat file_stat;

	if (stat(path, &file_stat)<0){
        fprintf(stderr, "Error: Something went wrong while reading file!\n");
        return -1;
    }

    if ((file_fd = open(path, O_RDONLY)) < 0){
        fprintf(stderr, "Error: Could not open file\n");
        return -1;
    }

	return OK;
}