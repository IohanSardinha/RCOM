#include "frame.h"

int tries;
enum s_frame_state_machine state_machine;
volatile bool TIME_OUT;

int send_time_out = 3, read_time_out = 7, send_tries = 3; 

char* header_to_string(unsigned char C)
{
	switch(C)
	{
		case C_SET:
			return "SET";
		case C_UA:
			return "UA";
		case C_DISC:
			return "DISC";
		case C_I_0:
			return "C_I, Ns = 0";
		case C_I_1:
			return "C_I, Ns = 1";
		case C_RR_0:
			return "RR, Ns = 0";
		case C_RR_1:
			return "RR, Ns = 1";
		case C_REJ_0:
			return "REJ, Ns = 0";
		case C_REJ_1:
			return "REJ, Ns = 1";
	}
	return "NONE";
}

void handleAlarm(){
	TIME_OUT = true;
}

unsigned char* s_frame(unsigned char A, unsigned char C)
{
	unsigned char* frame = malloc(sizeof(unsigned char)*S_FRAME_SIZE);
	frame[0] = FLAG;
	frame[1] = A;
	frame[2] = C;
	frame[3] = A^C;
	frame[4] = FLAG;
	return frame;
}

int send_s_frame(int fd,unsigned char A, unsigned char C)
{


	unsigned char* frame = s_frame(A,C);

	if(write(fd,frame,S_FRAME_SIZE) < 0)
		return -1;

	//for(int i = 0; i < 5; i++)
	//	printf("%x\n", frame[i]);

    free(frame);

    if(debug) printf("Sent: %s\n", header_to_string(C));

	return OK;
}

int send_s_frame_with_response(int fd, unsigned char A, unsigned char C, unsigned char response, unsigned char responder)
{
	int ret;
    unsigned char rcvd[1];
    unsigned char frame[5];

    tries = 0;
    state_machine = START_S;

    (void) signal(SIGALRM, handleAlarm);
    
    do
  	{
      	tries++;
	    TIME_OUT = false;

		if(send_s_frame(fd, A, C) != OK) return -1;

	    alarm(send_time_out);

	    state_machine = START_S;
	    
	    do{
	      ret = read(fd,rcvd,1);
	      if(TIME_OUT) 
	      	break;
	      if(ret == 0) continue;
	      //printf("%x\n", rcvd[0]);
	      change_s_frame_state(&state_machine, rcvd[0], frame, responder, response);
	    }while(state_machine!=STOP_S);
	    
  	}while(state_machine != STOP_S && tries<send_tries);

  	if(tries >= send_tries) return -1;

    if(debug) printf("Recieved response: %s\n", header_to_string(response));
	
	return OK;
}

int read_s_frame(int fd, unsigned char A, unsigned char C)
{
	unsigned char rcvd[1];
    unsigned char frame[5];
    
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

void change_s_frame_state(enum s_frame_state_machine* state, unsigned char rcvd, unsigned char* frame, unsigned char A, unsigned char C)
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
			else if(rcvd == C_RR_0)
			{
				*state = C_RCV;
				frame[2] = C_RR_0;	
			}
			else if(rcvd == C_RR_1)
			{
				*state = C_RCV;
				frame[2] = C_RR_1;	
			}
			else if(rcvd == C_REJ_0)
			{
				*state = C_RCV;
				frame[2] = C_REJ_0;	
			}
			else if(rcvd == C_REJ_1)
			{
				*state = C_RCV;
				frame[2] = C_REJ_1;	
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



void change_I_frame_state(enum i_frame_state_machine* state, unsigned char rcvd, unsigned char* frame, int n, int C)
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
			unsigned char par;
			int naointeressa;
			unsigned char * destuffedFrame= destuffing(frame, n+1,&par,&naointeressa);
			
			
			if (frame[n-1]== par){
			*state=STOP_I;
			
			}
			else if (frame[n-1]== 0x5e||frame[n-1]== 0x5d){
				if (frame[n-2]==ESC)
					*state=STOP_I;
			}
			else{*state=BCC_NOKI;}
		}
		else frame[n]=rcvd;
		//else esta a ler os dados
	}
}



unsigned char* destuffing (unsigned char * data, int tamanho,unsigned char * parity,int* numDados){

	unsigned char* fulltrama= malloc(sizeof(unsigned char)*tamanho);
	unsigned char* dados=malloc(sizeof(unsigned char)*tamanho);
	int n=0;
	unsigned char parityGiven;
	unsigned char parityCalculated;
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
					n++;
					fulltrama[actual]=FLAG;
					if (i==4){parityCalculated=FLAG;}
					else if (i>4){parityCalculated=parityCalculated^FLAG;}
				
				}
				
				else if (data[i+1]==0x5d){//dado igual a ESC
				//printf("somehow it came in here\n");
					dados[n]=ESC;
					n++;
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
				n++;
				if (i==4)
				{
					parityCalculated=data[i];
				}
				else if (i>4)
				{
					parityCalculated=parityCalculated^data[i];
				}	
					//parityCalculated=parityCalculated^data[i];
				
			}
		}
		actual++;
	
	}
	
	*parity=parityCalculated;
	*numDados=(n-1);
	free(fulltrama);
	
	return dados;

}

unsigned char REJTransform(int C){
	if (C==0)return C_REJ_0 ;
	else {return C_REJ_1;}

}



unsigned char RRTransform (int C){
	if (C==0)return C_RR_0;
	else {return C_RR_1;}


}

unsigned char* i_frame( unsigned char* data, unsigned char A, unsigned char C, int tamanho, int* frameSize){
	unsigned char parity=data[0];
	int oversize=0;
	
	for (int i=0; i<tamanho;i++){
		if (i!=0)parity=parity^data[i];
		if (data[i] == FLAG || data[i] == ESC){
			oversize++;		
		}
	}
	
	if (parity== FLAG || parity== ESC)oversize++;
	int size=sizeof(unsigned char)*(6+tamanho+oversize);
	
	unsigned char* frame= malloc (size);
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

int send_i_frame(int fd, unsigned char A, unsigned char C, unsigned char* data, int lenght)
{
	int frame_size, res;
	unsigned char* frame = i_frame(data, A, C, lenght, &frame_size);

	if(write(fd, frame, frame_size) < 0)
		return -1;

	free(frame);

	if(debug) printf("Sent: %s\n",header_to_string(C));

	return frame_size;
}

int send_i_frame_with_response(int fd, unsigned char A, unsigned char C, unsigned char* data, int lenght, int Ns)
{
	int ret, size;
    unsigned char rcvd[1];
	unsigned char frame[5];

    tries = 0;
    state_machine = START_S;

    (void) signal(SIGALRM, handleAlarm);
    
    do
  	{
	    tries++;
	    
	    TIME_OUT = false;

		if((size = send_i_frame(fd, A, C, data, lenght)) < 0) return -1;

	    alarm(send_time_out);

	    state_machine = START_S;
	    
	    do{
	      ret = read(fd,rcvd,1);
	      if(TIME_OUT)
	      	break;
	      if(ret == 0) continue;
	      change_s_frame_state(&state_machine, rcvd[0], frame, A, C_RET_I);
	    }while(state_machine!=STOP_S);

    	if(debug) printf("Recieved response: %s\n", frame[2] == -1 ? "NONE" : header_to_string(frame[2]));
	    
	    if((frame[2] == C_RR_0 && Ns == 0) || (frame[2] == C_RR_1 && Ns == 1) || (frame[2] == C_REJ_1) || (frame[2] == C_REJ_0))
		{
	    	state_machine = START_S;
			continue;
		}
	    
  	
  	}while(state_machine != STOP_S && tries<send_tries);

  	if(tries >= send_tries) return -1;

	
	return size;
}


int read_i_frame_with_response(int fd, unsigned char * packetbuff){

	int numBytes=0;
	static int packetB=0;	
	enum i_frame_state_machine state_machine = START_I;
    unsigned char rcvd[1];
    unsigned char* frame= malloc (sizeof(char)*(MAX_SIZE_FRAME));
    int res;
    int n=0;
    
    
    //time out para o read
    TIME_OUT = false;
    (void) signal(SIGALRM, handleAlarm);
    alarm(read_time_out);
    
    	    
    do
    {
      res = read(fd,rcvd,1);
      if (TIME_OUT)return -1;
      if (res==0)continue;
      if (state_machine==BCC_NOKI)
      {
      	if((send_s_frame(fd, A_TR, REJTransform(packetB)))!=OK)
      		return -2;      	
      	return -3;
      }
      change_I_frame_state(&state_machine, rcvd[0], frame, n, packetB);
      n++;
      alarm(read_time_out);
    }while(state_machine != STOP_I);
    
    if(state_machine == STOP_I){
	    char naointeressa;
		
		unsigned char* dstfd = destuffing(frame,n+1,&naointeressa,&numBytes);  //just to reuse the function really

		for(int i = 0; i < numBytes; i ++)
			packetbuff[i] = dstfd[i];

		packetB= (packetB +1)%2;	
		if((send_s_frame(fd, A_TR, RRTransform(packetB)))!=OK)return -2;
		
		
		return numBytes;
	
	}
	else return -1;

}

