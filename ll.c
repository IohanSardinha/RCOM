#include "ll.h"

int role, Ns = 0;
struct termios oldtio;

int llopen(int portN, int role_)
{
	
	struct termios newtio;
	char port[255];
	
	int fd;

	role = role_;

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
	newtio.c_cc[VTIME]    = 1;
	newtio.c_cc[VMIN]     = 0;


	tcflush(fd, TCIOFLUSH);

	if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		return -1;
	}

	printf("New termios structure set\n");

	switch(role_)
	{
		case TRANSMITTER:
			
			if(send_s_frame_with_response(fd,A_TR,C_SET,C_UA) != OK) return -1;
			break;

		case RECIEVER:
			if(read_s_frame(fd, A_TR, C_SET) != OK)return -1;
    		if(send_s_frame(fd, A_TR, C_UA) != OK )return -1;
    		break;	
	}

	return fd;
}


int llread(int fd, char* packetbuff){
	int numBytes=0;
	static int packetB=0;	
	enum i_frame_state_machine state_machine = START_I;
    char rcvd[1];
    char* frame= malloc (sizeof(char)*(MAX_SIZE_FRAME));
    char* stuffedpacket= malloc (sizeof(char)*(MAX_SIZE_FRAME));
    char* packet= malloc (sizeof(char)*(MAX_SIZE_PACKET));
    int res;
    int n=0;
    
    	    
    do
    {
      res = read(fd,rcvd,1);
      if (res<0){
      	if((send_s_frame(fd, A_TR, REJTransform(packetB)))!=OK)return -2;      	
      	return -1;}
      change_I_frame_state(&state_machine, rcvd[0], frame, n, packetB);
      n++;
    }while(state_machine != STOP_I);
    
    char naointeressa;
	unsigned char* dstfd = destuffing(frame,n+1,&naointeressa,&numBytes);  //just to reuse the function really
	
	memcpy(packetbuff,&dstfd, numBytes);

	for (int i =0; i < numBytes; i++){
		printf("%x\n", packetbuff[i]);
	}
	packetB= (packetB +1)%2;	
	if((send_s_frame(fd, A_TR, RRTransform(packetB)))!=OK)return -2;
	
		
	return numBytes;
}




int llwrite(int fd, char* buffer, int lenght)
{
	int res = send_i_frame_with_response(fd,A_TR, (Ns == 0)?C_I_0:C_I_1 , buffer, lenght, Ns);

	if(res < 0)
		return -1;
	
	Ns = (Ns +1) % 2;
	return res;
}


int llclose(int fd){
	switch(role)
	{
		case TRANSMITTER:
			if(send_s_frame_with_response(fd,A_TR,C_DISC,C_DISC) != OK) return -1;
			if(send_s_frame(fd, A_TR, C_UA) < 0) return -1;
			break;
		case RECIEVER:
			if(read_s_frame(fd,A_TR,C_DISC) < 0) return -1;
			if(send_s_frame_with_response(fd,A_TR,C_DISC, C_UA) != OK) return -1;
			break;
	}

	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    	perror("tcsetattr");
    	return -1;
  	}
    return close(fd);
}
