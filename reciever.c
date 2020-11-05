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
		
		if(reder<=0){
			printf("%d\n",reder);
			fprintf(stderr,"Error: Something went wrong while reading\n");
			char rm[255];
			sprintf(rm, "rm %s", path);
			//system(rm);
			return -1;
		}

		packetnumb++;
		
		packetresp=parseSendPacket(buffer,reder,path);
		
		if (packetresp==C_END)break;
			
	}
	return 0;
	
}

