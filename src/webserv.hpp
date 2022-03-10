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
#include <sys/wait.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

extern char **environ;

void	ft_fork(std::string path, char **tab, char **env);
void	ft_execve(std::string path, char **tab, char **env);
void	parceToEnv(char *conf);
void    parse_output_client(std::string & output);

#endif
