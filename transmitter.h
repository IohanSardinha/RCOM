#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"
#include "packet.h"

extern bool debug;

int transmitterMain(int fd, char* path);

int transmitData(int fd, int fd_file, struct stat stat_file);