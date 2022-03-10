#include "webserv.hpp"

/*
** What is a socket (in a large snens) ? (a combination of ip and port and protocol)
** What does it do ? (uniquely identifies the endpoint of a communication link between two application ports)
** The function socket(int domain, int type, int protocol) doesn't return a socket per se
** It creates an the endpoint for communication and return a file_descriptor that refers to that endpoint
*/

/*
** What is a blocking and non-blocking socket ?
** https://www.scottklement.com/rpg/socktut/nonblocking.html -> Good
*/
int   init_webserv_socket(sockaddr_in * sock_struct)
{
	int server_fd = -1;
	int	yes = 1, on = 1;

	/*
	** Returns a socket descriptor (endpoint)
	** only the protocol is specified (not yet the IP and PORT)
	** A socket_descriptor represents the socket but is not a socket in itself (it is a binary that acts as an index to the socket)
	*/
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "socket error" << std::endl;
		return (-1);
	}

	/*
	** Allows the application to reuse local address when the server restarts (CTRL-C + ./webserv) before wait time expires
	*/
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		printf("%s\n", strerror(errno));
		std::cerr << "setsockopt error" << std::endl;
		return (-1);
	}
	/*
	** Set socket to be non blocking. All the sockets for incoming connections will be also nonblocking
	*/
	if (ioctl(server_fd, FIONBIO, (char *)&on) < 0)
	{
		printf("%s\n", strerror(errno));
		std::cerr << "ioctl error" << std::endl;
		return (-1);
	}

	memset(sock_struct, 0, sizeof(*sock_struct));
	sock_struct->sin_family = AF_INET;
	sock_struct->sin_port = htons(8080);
	sock_struct->sin_addr.s_addr = inet_addr("127.0.0.1");

	/*
	** Gets a unique name for the socket
	** bind() assigns the address specified by the second argument to the socket referred to by the file descriptor sever_fd
	** it is here that the fd will be assigned an IP and PORT that he will afterwards listen to
	*/
	if (bind(server_fd, (sockaddr *)sock_struct, sizeof(*sock_struct)) < 0)
	{
		printf("%s\n", strerror(errno));
		std::cerr << "bind error" << std::endl;
		return (-1);
	}

	/*
	** Allows the server to accept incoming client connections
	** listen() marks the socket referred by server_fd as a passive socket that will be used to accept incoming connection with accept
	*/
	if (listen(server_fd, 42) < 0) 
	{ 
		printf("%s\n", strerror(errno));
		std::cerr << "listen error" << std::endl;
		return (-1);
	}

	return (server_fd);
}
