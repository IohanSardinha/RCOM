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

#define BAUDRATE B38400

int llopen(int port, int role);

int openTransmitter(int fd);

int openReciever(int fd);

void handleAlarm();

int llclose(int fd);