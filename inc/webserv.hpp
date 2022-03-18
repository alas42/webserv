#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// CPP Containers
#include <vector>
#include <list>
#include <map>

// CPP Headers
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>

// C Headers
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

// C System
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TRUE             1
#define FALSE            0
#define DEFAULT_CONFIG "data/example.conf"

#endif
