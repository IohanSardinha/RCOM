#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define TRANSMITTER 0 
#define RECIEVER 	1

#define FLAG	 0x7e
#define C_SET  	 0x03
#define C_UA	 0x07
#define C_DISC 	 0x0b
#define A_TR 	 0x03
#define A_RC 	 0x01
#define ESC 	 0x7d

int is_number(const char* str);

enum s_frame_state_machine{START_S,FLAG_RCV,A_RCV,C_RCV,BCC_OK,STOP_S};

enum i_frame_state_machine{START_I,FLAG_RCVI,A_RCVI,C_RCVI,BCC_OKI,STOP_I};

char* s_frame(char A, char C);

void change_s_frame_state(enum s_frame_state_machine* state, char rcvd, char* frame);