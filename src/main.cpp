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
	if (argc == 2)
		parceToEnv(argv[1]);
	else if (argc == 1) {
		std::cout << "need arg" << std::endl;
		return (0);
	}

	//std::string hello = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 200\n\n<html><body><h1><form action=\"php-cgi\" method=\"get\">Name: <input type=\"text\" name=\"name\"><br>E-mail: <input type=\"text\" name=\"email\"><br><input type=\"submit\"></form></body></html>";
	char 		**tab = NULL;
	sockaddr_in sock_struct;
	int 		server_fd;

	/*
	** Variables for multiple connections
	*/
	int 			new_socket = -1;
	int    			timeout;
  	struct pollfd	fds[200];
	char   			buffer[80];
	int				rc = 0, nfds = 1, current_size = 0, close_connection, end = FALSE, len, i, j, compress_array = FALSE;

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

	/*
	** Code issu de IBM
	*/
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
						if (errno != EWOULDBLOCK)
						{
							perror("  accept() failed");
							end = TRUE;
						}
						break ;
					}
					printf("  New incoming connection - %d\n", new_socket);
					fds[nfds].fd = new_socket;
					fds[nfds].events = POLLIN;
					nfds++;
				} while (new_socket != -1);
			}
			else
			{
				std::cout << "  Descriptor " << fds[i].fd << " is readable" << std::endl;
				close_connection = FALSE;
				/*******************************************************/
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */
				/*******************************************************/


					/*****************************************************/
					/* Receive data on this connection until the         */
					/* recv fails with EWOULDBLOCK. If any other         */
					/* failure occurs, we will close the                 */
					/* connection.                                       */
					/*****************************************************/
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  recv() failed");
							close_connection = TRUE;
						}
						break;
					}

					/*****************************************************/
					/* Check to see if the connection has been           */
					/* closed by the client                              */
					/*****************************************************/
					if (rc == 0)
					{
						printf("  Connection closed\n");
						close_connection = TRUE;
						break;
					}

					/*****************************************************/
					/* Data was received                                 */
					/*****************************************************/
					len = rc;
					printf("  %d bytes received\n", len);

					/*****************************************************/
					/* Echo the data back to the client                  */
					/*****************************************************/
					rc = send(fds[i].fd, buffer, len, 0);
					if (rc < 0)
					{
						perror("  send() failed");
						close_connection = TRUE;
						break;
					}
				if (close_connection)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}
			}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors              */

		/***********************************************************/
		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		/***********************************************************/
		if (compress_array)
		{
			compress_array = FALSE;
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for(j = i; j < nfds; j++)
					{
						fds[j].fd = fds[j+1].fd;
					}
					i--;
					nfds--;
				}
			}
		}
	}	while (end == FALSE);

	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
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
