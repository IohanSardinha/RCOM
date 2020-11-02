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


void change_I_frame_state(enum i_frame_state_machine* state, char rcvd, char* frame, int n, int C)
{
	if(*state == START_I)
	{
		if(rcvd == FLAG)
		{
			*state = FLAG_RCVI;
			frame[0] = FLAG;
			
		}
		//else keep state
	}
	else if(*state == FLAG_RCVI)
	{
		if(rcvd == A_TR)
		{
			*state = A_RCVI;
			frame[1] = A_TR;
		}
		else if(rcvd != FLAG)
			*state = START_I;

		//else keep state
	}
	else if(*state == A_RCVI)
	{
		if (C==0){
			if(rcvd == C0)
			{
				*state = C_RCVI;
				frame[2] = C0;
			}
			
			else if (rcvd == FLAG){
			*state= FLAG_RCVI;
			}
			
			else{*state = START_I;}
			
		}
		else if(C == 1)
		{
			if(rcvd == C1)
			{
				*state = C_RCVI;
				frame[2] = C1;
			}
			else if (rcvd == FLAG){
			*state= FLAG_RCVI;
			}
			
			else{*state = START_I;}
		}
		
	}
	else if(*state == C_RCVI)
	{
		if(rcvd == frame[1]^frame[2])
		{
			*state = BCC_OKI;
			frame[3] = rcvd;
		}
		else if(rcvd == FLAG)
		{
			*state = FLAG_RCVI;
		}
		else
		{
			*state = START_I;
		}
	}
	else if(*state == BCC_OKI)
	{
		
		if(rcvd == FLAG)
		{
			
			frame[n] = FLAG;
			char par;
			char * destuffedFrame= destuffing(frame, n+1,&par);
			
			
			if (frame[n-1]== par){
			*state=STOP_I;
			
			}
			else if (frame[n-1]== 0x5e||frame[n-1]== 0x5d){
				if (frame[n-2]==ESC)
					*state=STOP_I;
			}
		}
		else frame[n]=rcvd;
		//else esta a ler os dados
	}
}



char * destuffing (char * data, int tamanho,char * parity){

char * fulltrama= malloc(sizeof(char)*tamanho);
char * dados=malloc(sizeof(char)*tamanho);
int n=0;
char parityGiven;
char parityCalculated;
int actual=0;


	for (int i=0; i< tamanho; i++){
		if (i<4){fulltrama[actual]=data[i];}
		else{
			if (data[i]==ESC){ //tudo com esc
				if (data[i+2]==FLAG){  //se for o bcc2
				
					if (data[i+1]==0x5e){ //se bcc for FLAG
						parityGiven=FLAG;
					}
					else{parityGiven=ESC;}//se bcc for ESC
					
					fulltrama[actual]=parityGiven;
		
				}
				else if (data[i+1]==0x5e){//dado igual a FLAG
					dados[n]=FLAG;
					fulltrama[actual]=FLAG;
					if (i==4){parityCalculated=FLAG;}
					else if (i>4){parityCalculated=parityCalculated^FLAG;}
				
				}
				
				else if (data[i+1]==0x5d){//dado igual a ESC
				//printf("somehow it came in here\n");
					dados[n]=ESC;
					fulltrama[actual]=ESC;
					if (i==4){parityCalculated=ESC;}
					else if (i>4){parityCalculated=parityCalculated^ESC;}
				}
				i++;
				
			}
			
			//sem ESC
			else if (data[i]==FLAG){fulltrama[actual]=data[i];}//se for a flag
			else if (data[i+1]==FLAG){fulltrama[actual]=data[i];} //se for o bcc2
			else{								//dados with no need for stuffing
				dados[n]=data[i];
				fulltrama[actual]=data[i];
				
				if (i==4){parityCalculated=data[i];}
					else if (i>4){parityCalculated=parityCalculated^data[i];}
					
					
					//parityCalculated=parityCalculated^data[i];
				
			}
			n++;
		}
		actual++;
	
	}
	
*parity=parityCalculated;

}

char REJTransform(int C){

	if (C==0)return 0x01;
	else {return 0x81;}

}



char RRTransform (int C){

	if (C==0)return 0x05;
	else {return 0x85;}

}





