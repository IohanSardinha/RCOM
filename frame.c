#include "frame.h"

bool debug = false;

int tries;
enum s_frame_state_machine state_machine;
volatile bool TIME_OUT;

static int send_time_out = 3, read_time_out = 7, send_tries = 3; 

char* header_to_string(char C)
{
	switch(C)
	{
		case C_SET:
			return "SET";
		case C_UA:
			return "UA";
		case C_DISC:
			return "DISC";
	}
	return "";
}

void handleAlarm(){
	TIME_OUT = true;
}

char* s_frame(char A, char C)
{
	char* frame = malloc(sizeof(char)*S_FRAME_SIZE);
	frame[0] = FLAG;
	frame[1] = A;
	frame[2] = C;
	frame[3] = A^C;
	frame[4] = FLAG;
	return frame;
}

int send_s_frame(int fd,char A, char C)
{
	int ret;

	char* set_frame = s_frame(A,C);

	ret = write(fd,set_frame,S_FRAME_SIZE);

	if(ret < 0)
		return -1;

    free(set_frame);

    if(debug) printf("Sent: %s\n", header_to_string(C));

	return OK;
}

int send_s_frame_with_response(int fd, char A, char C, char response)
{
	int ret;

    tries = 0;
    state_machine = START_S;

    (void) signal(SIGALRM, handleAlarm);
    
    do
  	{
	    TIME_OUT = false;

		if(send_s_frame(fd, A, C) != OK) return -1;

	    alarm(send_time_out);

	    char rcvd[1];
	    char frame[5];

	    state_machine = START_S;

	    
	    do{
	      ret = read(fd,rcvd,1);
	      if(TIME_OUT)
	      { 
	      	tries++;
	      	break;
	      }
	      if(ret == 0) continue;
	      change_s_frame_state(&state_machine, rcvd[0], frame, (A == A_TR)?A_RC:A_TR, response);
	    }while(state_machine!=STOP_S);
	    
  	}while(state_machine != STOP_S && tries<send_tries);

  	if(tries >= send_tries) return -1;

    if(debug) printf("Recieved response: %s\n", header_to_string(response));
	
	return OK;
}

int read_s_frame(int fd, char A, char C)
{
	char rcvd[1];
    char frame[5];
    
    int res;

	state_machine = START_S;
	TIME_OUT = false;

    (void) signal(SIGALRM, handleAlarm);

    alarm(read_time_out);
    
    do
    {
      res = read(fd,rcvd,1);
      if(TIME_OUT) return -1;
      if(res == 0) continue;
      change_s_frame_state(&state_machine, rcvd[0], frame, A, C);
      alarm(read_time_out);
    }while(state_machine != STOP_S);


    if(debug) printf("Read: %s\n", header_to_string(C));
    
    return OK;
}

void change_s_frame_state(enum s_frame_state_machine* state, char rcvd, char* frame, char A, char C)
{
	switch(*state)
	{
		case START_S:
		
			if(rcvd == FLAG)
			{
				*state = FLAG_RCV;
				frame[0] = FLAG;
			}
			//else keep state
			
			break;
		
		case FLAG_RCV:
		
			if((rcvd == A_TR || rcvd == A_RC) && rcvd == A)
			{
				*state = A_RCV;
				frame[1] = A;
			}
			else if(rcvd != FLAG)
				*state = START_S;
			//else keep state
			
			break;

		case A_RCV:
		
			if((rcvd == C_SET || rcvd == C_UA || rcvd == C_DISC) && rcvd == C)
			{
				*state = C_RCV;
				frame[2] = C;
			}
			else if(rcvd == FLAG)
			{
				*state = FLAG_RCV;
			}
			else
				*state = START_S;
			
			break;

		case C_RCV:
		
			if(rcvd == frame[1]^frame[2])
			{
				*state = BCC_OK;
				frame[3] = rcvd;
			}
			else if(rcvd == FLAG)
			{
				*state = FLAG_RCV;
			}
			else
			{
				*state = START_S;
			}
			
			break;

		case BCC_OK:
		
			if(rcvd == FLAG)
			{
				*state = STOP_S;
				frame[4] = FLAG;
			}
			else *state=START_S;
			
			break;
	}
}