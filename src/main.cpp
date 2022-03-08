#include <arpa/inet.h>	//	htons ... inet_addr
#include <sys/select.h>	//  select
#include <poll.h>		//	poll
#include <sys/epoll.h>	//	epoll
#include <sys/socket.h>	//	socket setsockopt accept listen send getsockname
//#include <sys/event.h>
#include <sys/types.h>
#include <iostream>		//	std::cout
#include <functional>	//	std::bind
#include <unistd.h>		//	fcntl
#include <fcntl.h>		//	fcntl
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(int ac, char **av, char **env)
{
	(void)env;
	(void)ac;
	(void)av;
	std::cout<< "First thing first, obtain a real connection between\n"
		<< "the server and client." << std::endl;
	int server_fd, new_socket;
	sockaddr_in sock_struct;
	int len_addr = sizeof(sock_struct);
	long valread;
	std::string hello = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 16\n\n<h1>testing</h1>";

	/*
	** AF_INET = IP
	** SOCK_STREAM TOO
	** Protocol 0 because we don't care for now
	*/
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "socket error" << std::endl;
		return (1);
	}
	sock_struct.sin_family = AF_INET;
	sock_struct.sin_port = htons(8080);
	sock_struct.sin_addr.s_addr = inet_addr("127.0.0.1");
	//memset(sock_struct.sin_zero, '\0', sizeof sock_struct.sin_zero);
	if (bind(server_fd, (sockaddr *)&sock_struct, sizeof(sock_struct)) < 0)
	{
		std::cerr << "bind error" << std::endl;
		return (1);
	}

	if (listen(server_fd, 3) < 0) 
	{ 
		std::cerr << "listen error" << std::endl;
		return (1);
	}
	while (1)
	{
		if ((new_socket = accept(server_fd, (sockaddr *)&sock_struct, (socklen_t*)&len_addr)) < 0)
		{
			std::cerr << "accept error" << std::endl;
			return (1);        
		}

		char buffer[3000] = {0};
		/*char **tab = malloc(sizeof(char *) * 2);
		tab[1] = NULL;
		tab[0] = malloc(sizeof(char) * 30001);*/
        valread = read( new_socket , buffer, 30000);

        printf("%s\n", buffer );
	//	execve("/usr/bin/php-cgi", buffer, env);

        write(new_socket , hello.c_str() , strlen(hello.c_str()));
        printf("------------------Hello message sent-------------------\n");
		close(new_socket);
	}
	return (0);
}