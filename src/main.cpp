#include "webserv.hpp"

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
	char **tab = (char **)malloc(sizeof(char *) * 3);

	setenv("CONTENT_LENGTH", "1000", 1);
	setenv("CONTENT_TYPE", "NULL", 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("PATH_INFO", "./", 1);
	setenv("QUERY_STRING", "", 1);
	setenv("REMOTE_ADDR", "98.0.4758.102", 1);
	setenv("REQUEST_METHOD", "GET", 1);
	setenv("SCRIPT_NAME", "NULL", 1);
	setenv("SERVER_NAME", "NULL", 1);
	setenv("SERVER_PORT", "8080", 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("SERVER_SOFTWARE", "NULL", 1);
	setenv("REQUEST_URI", "index.html", 1);
	setenv("REDIRECT_STATUS", "200", 1);
	setenv("SCRIPT_FILENAME", "", 1);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "cannot create socket" << std::endl;
		return 0;
	}
	// memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);


	int yes=1;
	//char yes='1'; // use this under Solaris

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	if (bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0)
	{
		std::cerr << "bind failed" << std::endl;
        close(server_fd);
		return 0;
	}


	if (listen(server_fd, 3) < 0)
	{
		std::cerr << "In listen" << std::endl;
		exit(1);
	}

		tab[0] = strdup("/usr/bin/php-cgi");
		tab[1] = strdup("data/index.html");
		tab[2] = 0;

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

		std::string path = "/usr/bin/php-cgi";
		ft_fork(path, tab, env);
        printf("%s\n","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ");

        write(new_socket , hello.c_str() , hello.length());
        printf("------------------Hello message sent-------------------");
        close(new_socket);
	}

	return 0;
}
