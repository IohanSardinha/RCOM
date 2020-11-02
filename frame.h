#pragma once
#include "utils.h"

#define FLAG	 0x7e
#define C_SET  	 0x03
#define C_UA	 0x07
#define C_DISC 	 0x0b

#define C0 		 0x00
#define C1		 0x40

#define A_TR 	 0x03
#define A_RC 	 0x01

#define S_FRAME_SIZE 5

enum s_frame_state_machine{START_S,FLAG_RCV,A_RCV,C_RCV,BCC_OK,STOP_S};

enum i_frame_state_machine{START_I,FLAG_RCVI,A_RCVI,C_RCVI,BCC_OKI,STOP_I};

char* header_to_string(char C);

void handleAlarm();

char REJTransform(int C);

char RRTransform (int C);

char* destuffing (char* data, int tamanho,char* parity, int * numDados);

char* s_frame(char A, char C);

int send_s_frame(int fd, char A, char C);

int send_s_frame_with_response(int fd, char A, char C, char response);

int read_s_frame(int fd, char A, char C);

void change_s_frame_state(enum s_frame_state_machine* state, char rcvd, char* frame, char A, char C);

void change_I_frame_state(enum i_frame_state_machine* state, char rcvd, char* frame, int n, int C);
