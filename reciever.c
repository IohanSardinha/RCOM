#include "reciever.h"

int recieverMain(int fd, char* path)
{


	int reder;
	int packetnumb=0;
	int packetresp;
	
	while(1){
		unsigned char buffer[MAX_SIZE_FRAME] = {0};
		reder=llread(fd,buffer);
		if(reder<0){
			fprintf(stderr,"Error: Something went wrong while reading\n");
			return -1;
		}
		if(reder == 0)
			continue;
		
		packetnumb++;
		
		packetresp=parseSendPacket(buffer,reder,path);
		
		if (packetresp==C_END)break;
			
	}
	return 0;
	
}

