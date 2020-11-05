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

    char* file_name;
    (file_name = strrchr(path, '/')) ? ++file_name : (file_name = path);

    if(send_controll_packet(fd,C_START, stat_file.st_size, file_name) < 0)
    {
        fprintf(stderr, "Error: Failed to start sending data!\n");
        return -1;
    }

    if(transmitData(fd, fd_file, stat_file) < 0)
        return -1;
        

    if(send_controll_packet(fd,C_END, stat_file.st_size, file_name) < 0)
    {
        fprintf(stderr, "Error: Failed to end sending data!\n");
        return -1;
    }    

	return OK;
}

int transmitData(int fd, int fd_file, struct stat stat_file)
{
    int N = 0;
    int bytes;
    char buff[MAX_SIZE_PACKET];
    float progress=0;
    bool stopped = false;

    while((bytes = read(fd_file, buff, MAX_SIZE_PACKET - 4)) > 0)
    {
        char* packet = data_packet(N, bytes, buff);
        progress+=bytes;

        //Uncomment to simulate interuptions in the serial cable
        /*if(!stopped && (progress/stat_file.st_size) > 0.5)
        {
            printf("Stopping...\n");
            sleep(5);
            stopped = true;
            printf("Resuming...\n");
            sleep(1);
        }*/
        
        if(!debug)
        {
            system("clear");
            printf("New termios structure set\nConnection established!\n");
            print_progress(progress,stat_file.st_size);
        }
        int res;
        if((res = llwrite(fd, packet, (bytes+4 < MAX_SIZE_PACKET)? (bytes+4) : MAX_SIZE_PACKET)) < 0)
        {
            if(res == -1)
                fprintf(stderr, "Error: Could not write to file descriptor!\n");
            else if(res == -2)
                fprintf(stderr, "Error: Time-out, response took to long to arive!\n");
            else if(res == -3)
                fprintf(stderr, "Error: Could not read from file descriptor!\n");
            else
                fprintf(stderr, "Error: Something went wrong while transmitting data!\n");
            return -1;
        }
        
        free(packet);
        N++;
    }
    return OK;   
}