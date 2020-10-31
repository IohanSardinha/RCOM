#include "ll.h"

struct termios oldtio;

int llopen(int portN, int role){
	
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

	switch(role){
		case TRANSMITTER:
			break;
		case RECIEVER:
			break;
	}

}