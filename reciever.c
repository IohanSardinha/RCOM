#include "reciever.h"

int recieverMain(int fd, char* path)
{


	int reder;
	int packetnumb=0;
	int packetresp;
	
	while(1){
		
		unsigned char buffer[MAX_SIZE_FRAME] = {0};
		
		/*
		//teste do stop
		if (packetnumb==5){
		printf("timebeggin\n");
		sleep(5);
		printf("timeover\n");
		}
		
		*/
		packetnumb++;
		reder=llread(fd,buffer);

		if(reder == -3)
			continue;
		if(reder<=0){
			printf("%d\n",reder);
			fprintf(stderr,"Error: Something went wrong while reading\n");
			return -1;
		}
		
		packetresp=parseSendPacket(buffer,reder,path);
		
		if (packetresp==C_END)break;
			
	}
	return 0;
	
}

