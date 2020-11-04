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
    {
        fprintf(stderr, "Error: Something went wrong while transmitting data!\n");
        return -1;
    }    

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

    while((bytes = read(fd_file, buff, MAX_SIZE_PACKET - 4)) > 0)
    {
        char* packet = data_packet(N, bytes, buff);
        progress+=bytes;
        system("clear");
        printf("New termios structure set\nConnection established!\n");
        print_progress(progress,stat_file.st_size);
       
        if((llwrite(fd, packet, (bytes+4 < MAX_SIZE_PACKET)? (bytes+4) : MAX_SIZE_PACKET)) < 0)
            return -1;
        //printf("%f",progress);
        free(packet);

        N++;
    }
    return OK;   
}

void print_progress(float progress, int max){
	const char prefix[]= "Progress: [";
	const char suffix[]= "]";
	const size_t prefix_length= sizeof(prefix)-1;
	const size_t suffix_length = sizeof(suffix)-1;
	char * buffer= calloc(max+prefix_length+suffix_length+1,1); //+1 for end
	int i=0;
	
	strcpy(buffer, prefix);
	
	progress=progress/max*100;
	for (; i< 100; ++i){
	
		buffer[prefix_length+i]=i<progress?'#':' ';		
	}	
	
	strcpy(&buffer[prefix_length +i],suffix);
	printf("\b%c[2K\r%s %d%%\n", 27, buffer,(int)progress);
	
	fflush(stdout);
	free(buffer);
	

}


