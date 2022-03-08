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
#include <stdlib.h>

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
	std::string hello = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 16\n\n<h1>testing</h1>";
	std::string cgi_test = "GET /data/index.html HTTP/1.1 Host: 127.0.0.1:8080 Connection: keep-alive Cache-Control: max-age=0 sec-ch-ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"98\", \"Google Chrome\";v=\"98\" sec-ch-ua-mobile: ?0 sec-ch-ua-platform: \"Linux\" Upgrade-Insecure-Requests: 1 User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4758.102 Safari/537.36 Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9 Sec-Fetch-Site: none Sec-Fetch-Mode: navigate Sec-Fetch-User: ?1 Sec-Fetch-Dest: document Accept-Encoding: gzip, deflate, br Accept-Language: en-US,en;q=0.9";
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

		/*
		tab[2] = 0;
		tab[0] = strdup("/usr/bin/php-cgi");
		tab[1] = strdup(cgi_test.c_str());

        std::cout << cgi_test << std::endl;
		execve("/usr/bin/php-cgi", tab, env);
		*/

        write(new_socket , hello.c_str() , strlen(hello.c_str()));
		close(new_socket);
	}

	(void)env;
	(void)ac;
	(void)av;
	(void)tab;
	return (0);
}