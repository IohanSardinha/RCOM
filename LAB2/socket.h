#pragma once

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <errno.h> 
#include <netdb.h> 
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <stdbool.h>
#include "utils.h"

int ftp_connect(char* ip, int port);

int ftp_response(int fd, char* response);

int ftp_login(int fd, char* user, char* password);

int ftp_command(int fd, char* raw_command, char* args);

int parse_pasv_port(char* response, char* ip);