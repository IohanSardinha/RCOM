#define FLAG	 0x7e
#define C_SET  	 0x03
#define C_UA	 0x07
#define C_DISC 	 0x0b
#define A_EM 	 0x03
#define A_RC 	 0x01

enum s_frame_state_machine{START,FLAG_RCV,A_RCV,C_RCV,BCC_OK,STOP_MCHN};

void change_s_frame_state(enum s_frame_state_machine* state, char rcvd, char* frame)
{
	if(state == START)
	{
		if(rcvd == FLAG)
		{
			state = FLAG_RCV;
			frame[0] = FLAG;
		}
		//else keep state
	}
	else if(state == FLAG_RCV)
	{
		if(rcvd == A_EM)
		{
			state = A_RCV;
			frame[1] = A_EM;
		}
		else if(rcvd == A_RC)
		{
			state = A_RCV;	
			frame[1] = A_RC;
		}
		else if(rcvd != FLAG_RCV)
			state = START;
		//else keep state
	}
	else if(state == A_RCV)
	{
		
	}
	else if(state == C_RCV)
	{

	}
	else if(state == BCC_OK)
	{

	}
	else if(state == STOP_MCHN)
	{

	}
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
