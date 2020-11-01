#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "ll.h"
#include "reciever.h"
#include "transmitter.h"

extern bool debug;

int validateArgs(int argc, char const *argv[], int* role, int* port, char* path);

void printUsage();