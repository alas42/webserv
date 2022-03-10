#include "webserv.hpp"

void	free_tab(char **tab)
{
	int i = 0;
	while (tab[i] != 0)
	{
		free(tab[i++]);
	}
	free(tab);
	tab = NULL;
}

void	set_envs_cgi(void)
{
	setenv("DOCUMENT_ROOT", "mnt/nfs/homes/avogt/sgoinfre/webserv/data/", 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1); //variable : must be set to the dialect of CGI being used by the servre to communicate with the script
	setenv("SERVER_NAME", "127.0.0.1", 1); // variable : must be set to the name of the server host which the client request is directed
	setenv("SERVER_SOFTWARE", "webserv/1.0)", 1); // meta-variable : must be set to the name and version of the information server software
}

void set_sock_struct(sockaddr_in * sock_struct)
{
	sock_struct->sin_family = AF_INET;
	sock_struct->sin_port = htons(8080);
	sock_struct->sin_addr.s_addr = inet_addr("127.0.0.1");
}

int main(int argc, char **argv, char **env)
{
  if (argc == 2)
		parceToEnv(argv[1]);

	std::string hello = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 200\n\n<html><body><h1><form action=\"php-cgi\" method=\"get\">Name: <input type=\"text\" name=\"name\"><br>E-mail: <input type=\"text\" name=\"email\"><br><input type=\"submit\"></form></body></html>";
	sockaddr_in sock_struct;
	int 		server_fd;
	int 		new_socket = 0;
	int 		len_addr = sizeof(sock_struct);
	int 		yes = 1;
	pid_t		pid = -1;
	int			pipe[2];
	char 		**tab = NULL;
	
	set_envs_cgi();
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "socket error" << std::endl;
		return (1);
	}

	set_sock_struct(&sock_struct);

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		std::cerr << "setsockopt error" << std::endl;
		return (1);
	}
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
		tab = (char **)malloc(sizeof(char *) * 3);
		tab[0] = 0;
		tab[1] = 0;
		tab[2] = 0;
		pid = -1;

		if ((new_socket = accept(server_fd, (sockaddr *)&sock_struct, (socklen_t*)&len_addr)) < 0)
		{
			std::cerr << "accept error" << std::endl;
			return (1);        
		}

		std::string output(30000, 0);
		if (read(new_socket, &output[0], 30000-1) < 0) {
			std::cerr << "Failed to read data from socket.\n";
		}
		else
			std::cout << output << std::endl;
		parse_output_client(output);

		tab[0] = strdup("testers/cgi_self");
		tab[1] = strdup("data/form.php");
		tab[2] = 0;
		std::string path = "testers/cgi_self";
		ft_fork(path, tab, env);

    	printf("%s\n","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ");
		write(new_socket , hello.c_str() , strlen(hello.c_str()));
		printf("------------------Hello message sent-------------------");
		close(new_socket);
		free_tab(tab);
	}

	(void)env;
	(void)pipe;
	(void)tab;
	(void)pid;
	return (0);
}
