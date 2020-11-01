#include "ll.h"

int tries, role;
struct termios oldtio;
enum s_frame_state_machine state_machine;

volatile int TIME_OUT;
volatile int STOP_OPEN;

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

	printf("New termios structure set\n\n");

	switch(role_)
	{
		case TRANSMITTER:
			return openTransmitter(fd);
		case RECIEVER:
			return openReciever(fd);
	}
}

void handleAlarm(){
	if(role == TRANSMITTER)
	{
		if(state_machine == STOP_S)
		{
	      STOP_OPEN = true;
	      tries= 0 ;
	  	}
	    else
	    {
	      TIME_OUT = true;
	      tries++;
	  	}
  	}
  	else
  	{
  		TIME_OUT = true;
  	}
}

int openTransmitter(int fd){

	int ret;

    tries = 0;
    STOP_OPEN = false;
    state_machine = START_S;

    (void) signal(SIGALRM, handleAlarm);
    
    do
  	{
	    TIME_OUT = false;

		char* set_frame = s_frame(A_TR,C_SET);

		ret = write(fd,set_frame,5);

		if(ret < 0)
			return -1;

	    free(set_frame);

	    alarm(3);

	    char rcvd[1];
	    char frame[5];

	    state_machine = START_S;

	    while((!TIME_OUT)&& state_machine!=STOP_S)
	    {
	      ret = read(fd,rcvd,1);
	      if(ret == 0) continue;
	      if(TIME_OUT) break;
	      change_s_frame_state(&state_machine, rcvd[0], frame);
	    }
	    
  	}while(!STOP_OPEN && state_machine != STOP_S && tries<3);

  	if(tries >= 3) return -1;

	return fd;
}

int openReciever(int fd)
{
	state_machine = START_S;
    char rcvd[1];
    char frame[5];
    
    int res;

    (void) signal(SIGALRM, handleAlarm);

    alarm(7);
    
    do
    {
      res = read(fd,rcvd,1);
      if(TIME_OUT) return -1;
      if(res == 0) continue;
      change_s_frame_state(&state_machine, rcvd[0], frame);
      alarm(7);
    }while(state_machine != STOP_S);
    
    
    char* UA = s_frame(A_TR,C_UA);
    res = write(fd,UA,5);
    free(UA);

	return fd;
}

int llclose(int fd){
	switch(role)
	{
		case TRANSMITTER:
			//disconnectTransmitter()
			break;
		case RECIEVER:
			//disconnectReciever()
			break;
	}

	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    	perror("tcsetattr");
    	return -1;
  	}
    return close(fd);
}