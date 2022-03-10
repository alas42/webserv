#include "webserv.hpp"

extern char **environ;
void parse_output_client(std::string & output);


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

void	set_envs(void)
{
	setenv("DOCUMENT_ROOT", "mnt/nfs/homes/avogt/sgoinfre/webserv/data/", 1);
	setenv("DOCUMENT_NAME", "", 1);
	setenv("DOCUMENT_URL", "", 1);
	setenv("DOCUMENT_URI", "", 1);

	setenv("CONTENT_TYPE", "text/html", 1);
	setenv("CONTENT_LENGTH", "", 1);

	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("PATH_INFO", "/mnt/nfs/homes/avogt/sgoinfre/webserv/data/", 1);
	setenv("QUERY_STRING", "", 1); //What is after script_name in REQUEST_URI
	setenv("REMOTE_ADDR", "", 1);
	
	setenv("SCRIPT_NAME", "", 1); //Web path, requested file (ex: /cgi-bin/printenv.pl)
	setenv("SCRIPT_FILENAME", "/usr/bin/php-cgi", 1); //server_path, requested file (ex: C:/Program_Files...)
	
	setenv("SERVER_NAME", "127.0.0.1", 1);
	setenv("SERVER_PORT", "", 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("SERVER_SOFTWARE", "webserv", 1);
	
	setenv("REQUEST_METHOD", "", 1);
	setenv("REQUEST_URI", "", 1); //Web path, requested URI (ex: /cgi-bin/printenv.pl/foo/bar?var1=value1&var2=with%20percent%20encoding)
	
	setenv("REDIRECT_STATUS", "", 1);
}

void set_sock_struct(sockaddr_in * sock_struct)
{
	sock_struct->sin_family = AF_INET;
	sock_struct->sin_port = htons(8080);
	sock_struct->sin_addr.s_addr = inet_addr("127.0.0.1");
}

int main(int ac, char **av, char **env)
{
  if (argc != 2)
		return (0);
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
	
	set_envs();
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
		ft_fork(path, tab, env);
    printf("%s\n","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ");

    write(new_socket , hello.c_str() , hello.length());
    printf("------------------Hello message sent-------------------");
    close(new_socket);

    write(new_socket , hello.c_str() , strlen(hello.c_str()));
		close(new_socket);
		free_tab(tab);
	}

	(void)env;
	(void)ac;
	(void)av;
	(void)pipe;
	(void)tab;
	(void)pid;
	return (0);
}
