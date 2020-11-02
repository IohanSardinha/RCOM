#pragma once
#include "utils.h"
#include <string.h>

#define FLAG	 0x7e
#define C_SET  	 0x03
#define C_UA	 0x07
#define C_DISC 	 0x0b


#define C0 		 0x00
#define C1		 0x40


#define C_I_0    0x00
#define C_I_1	 0x40
#define C_RR_0   0x05
#define C_RR_1   0x06
#define C_REJ_0  0x01
#define C_REJ_1  0x81
#define C_RET_I	 -1

#define A_TR 	 0x03
#define A_RC 	 0x01

#define S_FRAME_SIZE 5

extern bool debug;

enum s_frame_state_machine{START_S,FLAG_RCV,A_RCV,C_RCV,BCC_OK,STOP_S};

enum i_frame_state_machine{START_I,FLAG_RCVI,A_RCVI,C_RCVI,BCC_OKI,BCC_NOKI,STOP_I};

char* header_to_string(unsigned char C);

void handleAlarm();

unsigned char REJTransform(int C);

unsigned char RRTransform (int C);

unsigned char* destuffing (unsigned char* data, int tamanho,unsigned char* parity, int * numDados);

unsigned char* s_frame(unsigned char A, unsigned char C);

int send_s_frame(int fd, unsigned char A, unsigned char C);

int send_s_frame_with_response(int fd, unsigned char A, unsigned char C, unsigned char response);

int read_s_frame(int fd, unsigned char A, unsigned char C);

void change_s_frame_state(enum s_frame_state_machine* state, unsigned char rcvd, unsigned char* frame, unsigned char A, unsigned char C);


void change_I_frame_state(enum i_frame_state_machine* state, unsigned char rcvd, unsigned char* frame, int n, int C);

unsigned char* i_frame(unsigned char* data, unsigned char A, unsigned char C,int tamanho,int* frameSize);

int send_i_frame(int fd, unsigned char A, unsigned char C, unsigned char* data, int lenght);

int send_i_frame_with_response(int fd, unsigned char A, unsigned char C, unsigned char* data, int lenght, int Ns);

int read_i_frame_with_response(int fd, unsigned char* packetbuff);

