#include "webserv.hpp"

void    connections(int server_fd)
{
    /*
	** Variables for multiple connections
	*/
	int 			new_socket = -1;
	int    			timeout;
  	struct pollfd	fds[200];
	char   			buffer[80];
	int				rc = 0, nfds = 1, current_size = 0, close_connection, end = FALSE, len, i, j, compress_array = FALSE;
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
}