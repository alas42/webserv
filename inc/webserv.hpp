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
#include <cstdlib>
#include <string>
#include <algorithm>
#include <sstream>

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
#define DEFAULT_CONFIG 	"data/example.conf"
// #define DEFAULT_INDEX 	"data/get_form.php"
#define DEFAULT_INDEX 	"data/webserv_default_index.html"

#define BUFFER_SIZE		1000000

	std::vector<std::string> mySplit(std::string s, std::string charset);
	int	pathIsFile(const std::string& path);
	int check_path(std::string path);
	int check_read_rights(std::string path);
	int check_execute_rights(std::string path);
	int check_wright_rights(std::string path);

#endif
