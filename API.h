#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FLAG	 0x7e
#define C_SET  	 0x03
#define C_UA	 0x07
#define C_DISC 	 0x0b
#define A_EM 	 0x03
#define A_RC 	 0x01
#define ESC 	0x7d

enum s_frame_state_machine{START_S,FLAG_RCV,A_RCV,C_RCV,BCC_OK,STOP_S};

enum i_frame_state_machine{START_I,FLAG_RCVI,A_RCVI,C_RCVI,BCC_OKI,STOP_I};

void printState(enum s_frame_state_machine state)
{
	if(state == START_S)
	{
		printf("START_S\n");
	}
	else if(state == FLAG_RCV)
	{
		printf("FLAG_RCV\n");
	}
	else if(state == A_RCV)
	{
		printf("A_RCV\n");
	}
	else if(state == C_RCV)
	{
		printf("C_RCV\n");
	}
	else if(state == BCC_OK)
	{
		printf("BCC_OK\n");
	}
	else if(state == STOP_S)
	{
		printf("STOP_S\n");
	}
}

void printStateI(enum i_frame_state_machine state)
{
	if(state == START_I)
	{
		printf("START_S\n");
	}
	else if(state == FLAG_RCVI)
	{
		printf("FLAG_RCV\n");
	}
	else if(state == A_RCVI)
	{
		printf("A_RCV\n");
	}
	else if(state == C_RCVI)
	{
		printf("C_RCV\n");
	}
	else if(state == BCC_OKI)
	{
		printf("BCC_OK\n");
	}
	else if(state == STOP_I)
	{
		printf("STOP_S\n");
	}
}

void change_s_frame_state(enum s_frame_state_machine* state, char rcvd, char* frame)
{
	if(*state == START_S)
	{
		if(rcvd == FLAG)
		{
			*state = FLAG_RCV;
			frame[0] = FLAG;
		}
		//else keep state
	}
	else if(*state == FLAG_RCV)
	{
		if(rcvd == A_EM)
		{
			*state = A_RCV;
			frame[1] = A_EM;
		}
		else if(rcvd == A_RC)
		{
			*state = A_RCV;	
			frame[1] = A_RC;
		}
		else if(rcvd != FLAG)
			*state = START_S;
		//else keep state
	}
	else if(*state == A_RCV)
	{
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
	}
	else if(*state == C_RCV)
	{
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
	}
	else if(*state == BCC_OK)
	{
		if(rcvd == FLAG)
		{
			*state = STOP_S;
			frame[4] = FLAG;
		}
		else *state=START_S;
	}
}


void change_I_frame_state(enum i_frame_state_machine* state, char rcvd, char* frame, char parity, int n)
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
		if(rcvd == A_EM)
		{
			*state = A_RCVI;
			frame[1] = A_EM;
		}
		else if(rcvd == A_RC)
		{
			*state = A_RCVI;	
			frame[1] = A_RC;
		}
		else if(rcvd != FLAG)
			*state = START_I;
		//else keep state
	}
	else if(*state == A_RCVI)
	{
		if(rcvd == C_SET)
		{
			*state = C_RCVI;
			frame[2] = C_SET;
		}
		else if(rcvd == C_UA)
		{
			*state = C_RCVI;
			frame[2] = C_UA;
		}
		else if(rcvd == C_DISC)
		{
			*state = C_RCVI;
			frame[2] = C_DISC;
		}
		else if(rcvd == FLAG)
		{
			*state = FLAG_RCVI;
		}
		else
			*state = START_I;
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
			
			
			if (frame[n-1]== parity){
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


char* i_frame( char* data, char A, char C,int tamanho,int* frameSize){
	char parity=data[0];
	int oversize=0;
	
	for (int i=0; i<tamanho;i++){
		if (i!=0)parity=parity^data[i];
		if (data[i] == FLAG || data[i] == ESC){
			oversize++;		
		}
	}
	
	if (parity== FLAG || parity== ESC)oversize++;
	
	char* frame= malloc (sizeof(char)*(6+tamanho+oversize));
	frame[0] = FLAG;
	frame[1] = A;
	frame[2] = C;
	frame[3] = A^C;
	
	int actual=4;
	//stuffing and counting parity
	for (int i=0; i<tamanho;i++){
		
		
		if (data[i] == FLAG){
			frame[i+actual]=0x7d;
			frame[i+actual+1]=0x5e;
			actual+=1;
			
		}else if (data[i]==ESC){
			frame[i+actual]=0x7d;
			frame[i+actual+1]=0x5d;
			actual+=1;
		
		}else{
			frame[i+actual]=data[i];
		}
		
	}
	if (parity==FLAG){
		frame[actual+tamanho]=0x7d;
		frame[actual+tamanho+1]=0x5e;
		actual+=1;
	}
	else if (parity==ESC){
		frame[actual+tamanho]=0x7d;
		frame[actual+tamanho+1]=0x5d;
		actual+=1;
	}
	else{frame[actual+tamanho]=parity;}
	
	frame[actual+tamanho+1]= FLAG;
	
	*frameSize=6+tamanho+oversize;
	
	return frame;

}

/*

char * destuffing (char * data, int trueness){

char * dados;
int n=0;
int BCCpedaco;

if (trueness==1)BCCpedaco=2;
else BCCpedaco=1;


	for (int i=4; i< (tamanho-BCCpedaco); i++){
		if (data[i]==ESC){}

	}
	


}

*/












