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

int main(int argc, char **argv, char **env)
{
  if (argc == 2)
		parceToEnv(argv[1]);

	std::string hello = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 200\n\n<html><body><h1><form action=\"php-cgi\" method=\"get\">Name: <input type=\"text\" name=\"name\"><br>E-mail: <input type=\"text\" name=\"email\"><br><input type=\"submit\"></form></body></html>";
	sockaddr_in sock_struct;
	int 		server_fd;
	int			end = FALSE;
	int 		new_socket = -1;
	int 		len_addr = sizeof(sock_struct);
	char 		**tab = NULL;
	int    		timeout;
  	struct pollfd fds[200];
	int			rc = 0, nfds = 1, current_size = 0;

	set_envs_cgi();
	if ((server_fd = init_webserv_socket(&sock_struct)) == -1)
	{
		std::cerr << "set_sock_struct error" << std::endl;
		return (1);
	}
	/*
	** Using structure from sys/poll library
	*/
 	memset(fds, 0 , sizeof(fds));
	fds[0].fd = server_fd;
  	fds[0].events = POLLIN;

	timeout = (3 * 60 * 1000);
	do
	{
		rc = poll(fds, nfds, timeout);
		if (rc < 0)
		{
			std::cerr << "poll error " << std::endl;
			break ;
		}else if(rc == 0)
		{
			std::cerr << "poll timeout " << std::endl;
			break ;
		}
		current_size = nfds;
		for (int i = 0; i < current_size; i++)
		{
			if (fds[i].revents == 0)
				continue ;
			if (fds[i].revents != POLLIN)
			{
				std::cerr << "poll unexpected result " << std::endl;
				end = TRUE;
				break ;
			}
			if (fds[i].fd == server_fd)
			{
				do
				{
					new_socket = accept(server_fd, NULL, NULL);
					if (new_socket < 0)
					{
						break ;
					}
				} while (new_socket != -1);
				
			}
		}
	}	while (end == FALSE);

	(void)env;
	(void)tab;
	return (0);
}

		/*tab = (char **)malloc(sizeof(char *) * 3);
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
		write(new_socket , hello.c_str() , strlen(hello.c_str()));
		printf("------------------Hello message sent-------------------");
		close(new_socket);
		free_tab(tab);*/
