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
#include <cstdlib>



int main(int ac, char **av, char **env)
{
	std::cout<< "First thing first, obtain a real connection between\n"
		<< "the server and client." << std::endl;
	/*
	** Essentials
	*/
	int server_fd, new_socket;
	sockaddr_in sock_struct;
	int len_addr = sizeof(sock_struct);
	long valread;

	/*
	** Tests
	*/
	std::string hello = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 200\n\n<html><body><h1><form action=\"php-cgi\" method=\"get\">Name: <input type=\"text\" name=\"name\"><br>E-mail: <input type=\"text\" name=\"email\"><br><input type=\"submit\"></form></body></html>";
	char **tab = (char **)malloc(sizeof(char *) * 3);

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

	/*
	** CGI ENVIRONMENT VARIABLES
	*/
	setenv("DOCUMENT_ROOT", "/mnt/nfs/homes/avogt/sgoinfre/webserv/data/", 1);
	setenv("CONTENT_TYPE", "null", 1);
	setenv("CONTENT_LENGTH", "22", 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("PATH_INFO", "/mnt/nfs/homes/avogt/sgoinfre/webserv/data/", 1);
	setenv("QUERY_STRING", "", 1);
	setenv("REMOTE_ADDR", "98.0.4758.102", 1); // have to change !!!
	setenv("REQUEST_METHOD", "GET", 1);
	setenv("SCRIPT_NAME", "", 1);
	setenv("SERVER_NAME", "messier", 1);
	setenv("SERVER_PORT", "8080", 1);
	setenv("SERVER_PROTOCOL", " HTTP/1.1", 1);
	setenv("SERVER_SOFTWARE", "webserv", 1);
	setenv("REQUEST_URI", "./index.html", 1);
	setenv("REDIRECT_STATUS", "200", 1);
	setenv("SCRIPT_FILENAME", "/usr/bin/php-cgi", 1);


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
        valread = read( new_socket , buffer, 30000);
		printf("%s\n", buffer);


	/*	tab[0] = strdup("/usr/bin/php-cgi");
		tab[1] = strdup("form.php");
		tab[2] = 0;*/
    //    std::cout << cgi_test << std::endl;
	//	execve("/usr/bin/php-cgi", tab, env);
		

        write(new_socket , hello.c_str() , strlen(hello.c_str()));
		close(new_socket);
	}

	(void)env;
	(void)ac;
	(void)av;
	(void)tab;
	return (0);
}