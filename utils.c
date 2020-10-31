#include "utils.h"

int is_number(const char* str){
	
	for(int i =0; str[i] != 0; i++)
	{
		if(!isdigit(str[i]))
			return 0;
	}
	
	return 1;
}

char* s_frame(char A, char C)
{
	char* frame = malloc(sizeof(char)*5);
	frame[0] = FLAG;
	frame[1] = A;
	frame[2] = C;
	frame[3] = A^C;
	frame[4] = FLAG;
	return frame;
}

void change_s_frame_state(enum s_frame_state_machine* state, char rcvd, char* frame)
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
		
			if(rcvd == A_TR)
			{
				*state = A_RCV;
				frame[1] = A_TR;
			}
			else if(rcvd == A_RC)
			{
				*state = A_RCV;	
				frame[1] = A_RC;
			}
			else if(rcvd != FLAG)
				*state = START_S;
			//else keep state
			
			break;

		case A_RCV:
		
			if(rcvd == C_SET)
			{
				*state = C_RCV;
				frame[2] = C_SET;
			}
			else if(rcvd == C_UA)
			{
				*state = C_RCV;
				frame[2] = C_UA;
			}
			else if(rcvd == C_DISC)
			{
				*state = C_RCV;
				frame[2] = C_DISC;
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