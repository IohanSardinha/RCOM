#include "transmitter.h"

int transmitterMain(int fd, char* path)
{
	int fd_file;
	struct stat stat_file;

	if (stat(path, &stat_file) < 0){
        fprintf(stderr, "Error: Something went wrong while reading file!\n");
        return -1;
    }

    if ((fd_file = open(path, O_RDONLY)) < 0){
        fprintf(stderr, "Error: Could not open file\n");
        return -1;
    }

    if(send_controll_packet(fd,C_START, stat_file.st_size, path) < 0)
    {
        fprintf(stderr, "Error: Failed to start sending data!\n");
        return -1;
    }

    if(transmitData(fd, fd_file, stat_file) < 0)
    {
        fprintf(stderr, "Error: Something went wrong while transmitting data!\n");
        return -1;
    }    

    if(send_controll_packet(fd,C_END, stat_file.st_size, path) < 0)
    {
        fprintf(stderr, "Error: Failed to end sending data!\n");
        return -1;
    }    

	return OK;
}

int transmitData(int fd, int fd_file, struct stat stat_file)
{
    
    return OK;   
}