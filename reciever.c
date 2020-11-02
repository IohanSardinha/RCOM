#include "reciever.h"

int recieverMain(int fd, char* path)
{


	int reder;
	int packetnumb=0;
	int packetresp;
	
	while(1){
		char* buffer= malloc (sizeof(char)*MAX_SIZE_PACKET);
		reder=llread(fd,buffer);
		if(reder<0)return -1;
		packetnumb++;
		printf("%d",reder);
		
		for (int i =0; i < reder; i++){
			printf("a%x\n", buffer[i]);
		}
			
	}
	return 0;
	
}

