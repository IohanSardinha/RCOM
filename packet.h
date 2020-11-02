#pragma once

#include "ll.h"
#include "utils.h"

#define C_DATA 1
#define C_START 2
#define C_END 	3

#define T_FILE_SIZE 0
#define T_FILE_NAME 1

int send_controll_packet(int fd, char C, int T1, char* T2);