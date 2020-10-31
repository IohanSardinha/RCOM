#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

#define OK 0

int validateArgs(int argc, char const *argv[], int* role, int* port, char* path);

void printUsage();