#include "reciever.h"

int recieverMain(int fd, char* path)
{


	int reder;
	int packetnumb=0;
	int packetresp;
	
	while(1){
		
		unsigned char buffer[MAX_SIZE_FRAME] = {0};
		
		
		reder=llread(fd,buffer);

		if(reder == -3)
			continue;
		
		if(reder <= 0){
			if(reder == -1)
				fprintf(stderr,"Error: Time-out, data took to long to arive!\n");
			else if(reder == -2 || reder == -4)
				fprintf(stderr,"Error: Could not write to file descriptor!\n");
			else
				fprintf(stderr,"Error: Something went wrong while reading\n");
			return -1;
		}

		packetnumb++;
		
		packetresp=parseSendPacket(buffer,reder,path);
		
		if (packetresp==C_END)break;
			
	}
	return 0;
	
}

