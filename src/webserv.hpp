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
#include <list>
#include <sys/wait.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#define TRUE             1
#define FALSE            0

extern char **environ;

void	ft_fork(std::string path, char **tab, char **env);
void	ft_execve(std::string path, char **tab, char **env);
void	parceToEnv(char *conf);
void    parse_output_client(std::string & output);
int     init_webserv_socket(sockaddr_in * sock_struct);

#endif
