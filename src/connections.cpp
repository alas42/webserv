#include "../inc/webserv.hpp"

/*
** the poll() functions is an observer of a tab of pollfd that contains their current fds and the events that is happening
** at each one of them
** can we listen to multiple ports ? yes, theorically we can have multiplt "server_fds"
** and set the appropriate fd in fds[n].fd to the good one after an accept
*/
void    connections(std::vector<int> & server_fds)
{
    /*
	** Variables for multiple connections
	*/
	int 			new_socket = -1;
	int    			timeout;
  	struct pollfd	fds[200];
	char   			buffer[500];
	int				rc = 0, nfds = (int)server_fds.size(), current_size = 0, close_connection, end = FALSE, len, i, j, compress_array = FALSE;

    /*
	** Using structure from sys/poll library
	*/
 	memset(fds, 0, sizeof(fds));
	for(size_t i = 0; i < server_fds.size(); i++)
	{
		fds[i].fd = server_fds[i];
		fds[i].events = POLLIN;
	}

	/*
	** Code issu de IBM
	*/
	timeout = (0.5 * 60 * 1000); //I guess every client should have it's own timeout before the connection closes
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
			if (fds[i].fd == server_fds[0] || fds[i].fd == server_fds[1])
			{
				do
				{
					new_socket = accept(fds[i].fd, NULL, NULL);
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

				rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
				if (rc < 0)
				{
					/* Checking the value of errno after read or write is FORBIDDEN 
					** if (errno != EWOULDBLOCK)
					** {
					**		perror("  recv() failed");
					**		close_connection = TRUE;
					**	}
					*/
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
				}

				/*****************************************************/
				/* Data was received                                 */
				/*****************************************************/
				len = rc;
				printf("  %d bytes received\n", len);
				write(1, buffer,rc);
				/*
				** Echo the data back to the client (uselfull for telnet, not for browser)
				**
				**	rc = send(fds[i].fd, buffer, len, 0);
				**	if (rc < 0)
				**	{
				**		perror("  send() failed");
				**		close_connection = TRUE;
				**	}
				*/
				if (close_connection)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}
			}
		}

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
}
