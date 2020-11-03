#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "frame.h"

#define BAUDRATE B38400

extern int read_time_out; 

int llopen(int port, int role);


int llread(int fd, unsigned char* packetbuff);

void handleAlarm();

int llwrite(int fd, unsigned char* buffer, int lenght);


int llclose(int fd);
