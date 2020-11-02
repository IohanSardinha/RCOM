#include "packet.h"

int send_controll_packet(int fd, char C, int T1, char* T2)
{
	int L1 = sizeof(T1);
	int L2 = strlen(T2);
	int size = 5 + L1 + L2;

	unsigned char* packet  = malloc(sizeof(char)*size);

	packet[0] = C;
	packet[1] = T_FILE_SIZE;
	packet[2] = L1;
	memcpy(&packet[3], &T1, L1);
	packet[3+L1] = T_FILE_NAME;
	packet[3+L1+1] = L2;
	memcpy(&packet[5+L1], T2, L2);

	if(llwrite(fd, packet, size) < 0) return -1;

	free(packet);

	return OK;
}

char* data_packet(int N, int bytes, char* buff)
{
	unsigned char* packet = malloc(sizeof(char)*MAX_SIZE_PACKET);
    packet[0] = C_DATA;
    packet[1] = N % 255;
    packet[2] = bytes/256;
    packet[3] = bytes%255;

    memcpy(&packet[4], buff, bytes);

    for(int i =0; i < bytes+4; i++)
    	printf("%x:", packet[i]);
    printf("\n");

    return packet;
}



int parseSendPacket(unsigned char* packet, int numB, char * path){
	for(int i = 0; i < numB; i++)
		printf("%x:", packet[i]);
	printf("\n");

	static int file;
	unsigned size;
	if (packet[0]==C_START){
		if ((file=open(path,O_RDWR | O_CREAT,0777))<0)return -1;
		return C_START;
	}
	
	else if (packet[0]==C_END){
		if (close(file)<0)return -1;
		return C_END;
	
	}
	
	else if (packet[0]==C_DATA){
		size= (unsigned char)packet[3]+256*((unsigned char)packet[2]);
		if (write(file,&packet[4],size)<0)return -1;
		return C_DATA;
	
	}
	else{return -1;}

}


