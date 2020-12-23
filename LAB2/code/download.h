#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 #include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "utils.h"
#include "socket.h"

int paserArgs(char* url, char* fields[]);

int download(char* fields[]);