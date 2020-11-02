#include "reciever.h"

int recieverMain(int fd, char* path)
{
	sleep(2);
	send_s_frame(fd, A_TR, C_RR_1);
	sleep(2);
	send_s_frame(fd, A_TR, C_RR_0);
	
	return OK;
}