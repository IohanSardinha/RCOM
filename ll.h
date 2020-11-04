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

#define BAUDRATE0 B38400
#define BAUDRATE1 B9600
#define BAUDRATE2 B19200
#define BAUDRATE3 B38400
#define BAUDRATE4  B57600
#define BAUDRATE5  B115200
#define BAUDRATE6  B230400
#define BAUDRATE7  B460800
#define BAUDRATE8  B500000

extern int read_time_out; 
extern bool debug;

int llopen(int port, int role);


int llread(int fd, unsigned char* packetbuff);

void handleAlarm();

int llwrite(int fd, unsigned char* buffer, int lenght);


int llclose(int fd);
