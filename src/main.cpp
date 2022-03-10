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

int main(int argc, char *argv[], char **env)
{
	sockaddr_in sock_struct; //WEBSERVER CLASS
	int 		server_fd; //WEBSERVER CLASS

	if (argc == 2)
		parseToEnv(argv[1]);
	else if (argc == 1)
	{
		std::cout << "need arg" << std::endl;
		return (1);
	}

	set_envs_cgi();
	if ((server_fd = init_webserv_socket(&sock_struct)) == -1)
	{
		std::cerr << "set_sock_struct error" << std::endl;
		return (1);
	}
	connections(server_fd);

	(void)env;
	return (0);
}

		/*char 		**tab = NULL;
		tab = (char **)malloc(sizeof(char *) * 3);
		tab[0] = 0;
		tab[1] = 0;
		tab[2] = 0;

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
		//std::string hello = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 200\n\n<html><body><h1><form action=\"php-cgi\" method=\"get\">Name: <input type=\"text\" name=\"name\"><br>E-mail: <input type=\"text\" name=\"email\"><br><input type=\"submit\"></form></body></html>";
		write(new_socket , hello.c_str() , strlen(hello.c_str()));
		printf("------------------Hello message sent-------------------");
		close(new_socket);
		free_tab(tab);*/
