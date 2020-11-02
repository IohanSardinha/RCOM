#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#define TRANSMITTER 0 
#define RECIEVER 	1

#define OK 0
#define ESC 	 0x7d


#define MAX_SIZE_FRAME 64
#define MAX_SIZE_PACKET  32


int is_number(const char* str);
