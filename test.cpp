#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>



int main(int argc, char **argv, char **env) {

	(void)argc;
	(void)argv;
	(void)env;

	int server_fd;
	long valread;
	struct sockaddr_in address;
	socklen_t address_len;
	int PORT = 8080;
	int new_socket;
	std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	// char **tab = (char **)malloc(sizeof(char *) * 3);

	char test[] = { "CONTENT_LENGTH=12",
					"CONTENT_TYPE=NULL",
					"GATEWAY_INTERFACE=CGI/1.1",
					"PATH_INFO=./",
					"QUERY_STRING=index.php",
					"REMOTE_ADDR=localhost",
					"REQUEST_METHOD=GET",
					"SCRIPT_NAME=NULL",
					"SERVER_NAME=NULL",
					"SERVER_PORT=8080",
					"SERVER_PROTOCOL=HTTP/1.1",
					"SERVER_SOFTWARE=NULL",
					"REQUEST_URI=./",
					"REDIRECT_STATUS=200",
					"SCRIPT_FILENAME=/usr/bin/php-cgi"};

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "cannot create socket" << std::endl;
		return 0;
	}
	// memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	if (bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0)
	{
		std::cerr << "bind failed" << std::endl;
		return 0;
	}


	if (listen(server_fd, 3) < 0)
	{
		std::cerr << "In listen" << std::endl;
		exit(1);
	}

		// tab[2] = 0;
		// tab[0] = strdup("/usr/bin/php-cgi");
		// tab[1] = strdup(cgi_test.c_str());

		// char *arguments[] = { "ps", "index.php", NULL};
	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len)) < 0 )
		{
			std::cerr << "In accept" << std::endl;
			exit(1);
		}

		char buffer[3000] = {0};
        valread = read( new_socket , buffer, 3000);
        printf("%s\n", buffer);
        printf("%s\n","---------------------------------------------------------------" );
		execve("/usr/bin/php-cgi", buffer, test);

        write(new_socket , hello.c_str() , hello.length());
        printf("------------------Hello message sent-------------------");
        close(new_socket);
	}

	return 0;
}
