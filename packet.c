#include "packet.h"

int send_controll_packet(int fd, char C, int T1, char* T2)
{
	int L1 = sizeof(T1);
	int L2 = strlen(T2);
	int size = 5 + L1 + L2;

	char* packet  = malloc(sizeof(char)*size);

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