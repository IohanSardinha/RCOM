#include "frame.h"

int tries;
enum s_frame_state_machine state_machine;
volatile bool TIME_OUT;
int line_number = 0;

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

void printState(enum i_frame_state_machine state)
{
	switch(state)
	{
		case START_I:
			printf("START_I\n");
			break;
		case FLAG_RCVI:
			printf("FLAG_RCVI\n");
			break;
		case A_RCVI:
			printf("A_RCVI\n");
			break;
		case C_RCVI:
			printf("C_RCVI\n");
			break;
		case BCC_OKI:
			printf("RECIEVING_DATA\n");
			break;
		case BCC_NOKI:
			printf("BCC_NOT OK\n");
			break;	
		case STOP_I:
			printf("STOP_I\n");
			break;
	}
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

    free(frame);

    if(debug){ printf("%d:\tSent: %s\n", line_number, header_to_string(C)); line_number++;}

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
	      change_s_frame_state(&state_machine, rcvd[0], frame, responder, response);
	    }while(state_machine!=STOP_S);
	    
	    
  	}while(state_machine != STOP_S && tries<send_tries);

  	if(tries >= send_tries) return -1;

    if(debug) {printf("%d:\tRecieved response: %s\n",line_number, header_to_string(response));line_number++;}

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


    if(debug){ printf("%d:\tRead: %s\n",line_number, header_to_string(C));line_number++;}

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


void change_i_frame_state(enum i_frame_state_machine* state, unsigned char rcvd, unsigned char* frame, int* n, int Ns)
{
	if(*state == START_I)
	{
		*n = 0;
		if(rcvd == FLAG)
		{
			*state = FLAG_RCVI;
			frame[0] = FLAG;	
			*n = 1;	
		}
		//else keep state
	}
	else if(*state == FLAG_RCVI)
	{
		if(rcvd == A_TR)
		{
			*state = A_RCVI;
			frame[1] = A_TR;
			*n = 2;
		}
		else if(rcvd != FLAG)
			*state = START_I;

		//else keep state
	}
	else if(*state == A_RCVI)
	{
		if (Ns==0){
			if(rcvd == C0)
			{
				*state = C_RCVI;
				frame[2] = C0;
				*n = 3;
			}
			
			else if (rcvd == FLAG)
				*state= FLAG_RCVI;
			
			else
				*state = START_I;
			
		}
		else if(Ns == 1)
		{
			if(rcvd == C1)
			{
				*state = C_RCVI;
				frame[2] = C1;
				*n = 3;
			}
			else if (rcvd == FLAG)
				*state= FLAG_RCVI;
			
			else
				*state = START_I;
		}
		
	}
	else if(*state == C_RCVI)
	{
		if(rcvd == frame[1]^frame[2])
		{
			*state = BCC_OKI;
			frame[3] = rcvd;
			*n = 4;
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
			*state = STOP_I;
		else
		{
			if(*n > MAX_SIZE_FRAME)
			{
				printf("MAX SIZE REACHED\n");
				*state = START_I;
			}
			else
			{
			 	frame[*n]=rcvd;
			 	(*n)++;
			}
		}
		//else esta a ler os dados
	}
}



unsigned char* destuffing (unsigned char * data, int tamanho, unsigned char* parity,int* numDados){

	unsigned char* fulltrama = malloc(sizeof(unsigned char)*tamanho);
	unsigned char* dados = malloc(sizeof(unsigned char)*tamanho);
	int n = 0;
	unsigned char parityGiven;
	unsigned char parityCalculated;
	int actual=0;


	for (int i=0; i< tamanho; i++){
		if (i < 4)
			fulltrama[actual]=data[i];

		else{
			if (data[i]==ESC) //tudo com esc
			{
				if (data[i+2]==FLAG)//se for o bcc2
				{
				
					if (data[i+1]==0x5e) //se bcc for FLAG
					{
						parityGiven=FLAG;
					}
					else //se bcc for ESC
					{
						parityGiven=ESC;
					}
					
					fulltrama[actual]=parityGiven;
		
				}
				else if (data[i+1]==0x5e) //dado igual a FLAG
				{
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
				else if (i>4 && i<tamanho-1)
				{
					parityCalculated = parityCalculated^data[i];
				}	
					//parityCalculated=parityCalculated^data[i];
				
			}
		}
		actual++;
	}
	
	*parity = parityCalculated;
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
	
	unsigned char* frame= malloc(sizeof(unsigned char)*size);
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

void corrupt(unsigned char* frame, int size)
{
	int corrupted_count = rand()%size;

	for (int n = 0; n < corrupted_count; n++)
	{
		int i = rand()%size;
		frame[i] = frame[i] & 0xaa; 
	}
}

int send_i_frame(int fd, unsigned char A, unsigned char C, unsigned char* data, int lenght)
{
	int frame_size, res;
	unsigned char* frame = i_frame(data, A, C, lenght, &frame_size);

	/*if(rand() % 100 < 10)
	{
		printf("CORRUPTED ::: ");
		corrupt(frame,frame_size);
	}

	for (int i = 0; i < frame_size; ++i)
		printf("%x:", frame[i]);
	printf("\n");*/
	
	
	if(write(fd, frame, frame_size) < 0)
		return -1;

	free(frame);

	if(debug){ printf("%d:\tSent: %s\n",line_number, header_to_string(C)); line_number++;}

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
  		frame[0] = -1;frame[1] = -1;frame[2] = -1;frame[3] = -1;frame[4] = -1; 

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
	      if(ret < 0)
	      {
	      	printf("WRONG READING\n"); 
	      	return -1;
	      }
	      change_s_frame_state(&state_machine, rcvd[0], frame, A, C_RET_I);
	    }while(state_machine!=STOP_S);

    	if(debug){ printf("%d:\tRecieved response: %s\n",line_number, frame[2] == -1 ? "NONE" : header_to_string(frame[2])); line_number++;}
	    
	    if((frame[2] == C_RR_0 && Ns == 0) || (frame[2] == C_RR_1 && Ns == 1) || (frame[2] == C_REJ_1) || (frame[2] == C_REJ_0))
		{
	    	state_machine = START_S;
			continue;
		}
	    
  	
  	}while(state_machine != STOP_S && tries<send_tries);

  	if(tries >= send_tries)
  	{
		printf("GAVE UP\n");
  	 	return -1;
	}
	
	return size;
}

/*
frame[n] = FLAG;
			unsigned char par;
			int naointeressa;
			unsigned char * destuffedFrame= destuffing(frame, n+1,&par,&naointeressa);
			
			if (frame[n-1]== par)
				*state=STOP_I;
			
			else if (frame[n-1]== 0x5e||frame[n-1]== 0x5d){
				
				if (frame[n-2]==ESC)
					*state=STOP_I;
			}
			else
			{
				printf("BCC2 WRONG\n");
				*state=BCC_NOKI;
			}
*/


int read_i_frame_with_response(int fd, unsigned char * buffer){

	static int Ns = 0;

	int frame_size = 0, res;

	enum i_frame_state_machine state_machine = START_I;

    unsigned char rcvd[1];
    unsigned char frame[MAX_SIZE_FRAME] = {0};
    
    unsigned char BCC2;
	unsigned char* frame_data_fields;
    int data_size;


    TIME_OUT = false;
    
    (void) signal(SIGALRM, handleAlarm);
    
    alarm(read_time_out);
    	    
    do
    {
      res = read(fd,rcvd,1);
      
      if (TIME_OUT)	return -1;
      if (res==0)	continue;
      
      change_i_frame_state(&state_machine, rcvd[0], frame, &frame_size, Ns);
      
      alarm(read_time_out);
    
    }while(state_machine != STOP_I);

    frame_data_fields = destuffing(frame, frame_size, &BCC2, &data_size);  //just to reuse the function really

    if(frame[frame_size-1] == BCC2)
    {

    	for(int i = 0; i < data_size; i ++)
			buffer[i] = frame_data_fields[i];

		if(debug){ printf("%d:\tRecieved frame, Ns= %d\n",line_number, Ns); line_number++;}

		Ns = (Ns + 1) % 2;	
		
		if((send_s_frame(fd, A_TR, RRTransform(Ns))) != OK) return -2;
		
		return data_size;

    }
	else
	{
		if((send_s_frame(fd, A_TR, REJTransform(Ns))) != OK) return -4;

		return -3;
	}	

}

