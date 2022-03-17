#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <sys/wait.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include "../src/config/Config.hpp"
#include <errno.h>

#define TRUE             1
#define FALSE            0

void	parceToEnv(const char *conf);

#endif
