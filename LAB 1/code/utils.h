#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <time.h>


#define CORUPTION_FREQUENCY 0.1

#define TRANSMITTER 0 
#define RECIEVER 	1

#define OK 0
#define ESC 	 0x7d


#define MAX_SIZE_PACKET 450 
#define MAX_SIZE_FRAME ((MAX_SIZE_PACKET *2))


int is_number(const char* str);
void print_progress(float progress, int max);
void corrupt(unsigned char* frame, int size);