#include "ll.h"

struct termios oldtio;

int llopen(int portN, int role)
{
	
	struct termios newtio;
	char port[255];
	
	int fd;

	sprintf(port, "/dev/ttyS%d", portN);

	fd = open(port, O_RDWR | O_NOCTTY );
	if (fd <0) {
		perror(port); 
		return -1; 
	}

	if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
		perror("tcgetattr");
		return -1;
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	newtio.c_lflag = 0;
	newtio.c_cc[VTIME]    = 0;
	newtio.c_cc[VMIN]     = 1;


	tcflush(fd, TCIOFLUSH);

	if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		return -1;
	}

	printf("New termios structure set\n\n");

	switch(role)
	{
		case TRANSMITTER:
			return openTransmitter(fd);
		case RECIEVER:
			return openReciever(fd);
	}
}

void handleAlarm(){
	
}

int openTransmitter(int fd){

	int ret;

	char* set_frame = s_frame(A_TR,C_SET);

	ret = write(fd,set_frame,5);

    free(set_frame);

    (void) signal(SIGALRM, handleAlarm);

	return fd;
}

int openReciever(int fd){


	return fd;
}