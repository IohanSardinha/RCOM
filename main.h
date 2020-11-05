#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "ll.h"
#include "reciever.h"
#include "transmitter.h"

extern bool debug;

extern int send_time_out; 
extern int read_time_out; 
extern int send_tries; 

int validateArgs(int argc, char const *argv[], int* role, int* port, char* path);

void printUsage();
