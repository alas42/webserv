# webserv
This is when you finally understand why a URL starts with HTTP

curl command = curl -H "Transfer-Encoding: chunked" -F "fileToUpload=@www/site1/imgs/background.jpg" -F "submit=1" http://localhost:8080/upload.php

https://developer.mozilla.org/fr/docs/Web/HTTP/Basics_of_HTTP


---------------------------------------Parsing du fichier de configuration------------------------------------


---------------------------------------Fonctionnement des sockets------------------------------------------------

- Concept de socket et integration client server :
https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
https://www.cas.mcmaster.ca/~qiao/courses/cs3mh3/tutorials/socket.html


- Systeme de poll:
https://suchprogramming.com/epoll-in-3-easy-steps/


----------------------------------------Gestions des requetes/reponses http---------------------------------------

- Erreurs HTTP:
https://developer.mozilla.org/fr/docs/Web/HTTP/Status


- Variable Envs pour CGI
https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.1



CGI:
http://didier.deleglise.free.fr/web_dyn/cgi/cgi.htm

/*
** What is a socket (in a large sens) ? (a combination of ip and port and protocol)
** What does it do ? (uniquely identifies the endpoint of a communication link between two application ports)
** The function socket(int domain, int type, int protocol) doesn't return a socket per se
** It creates an the endpoint for communication and return a file_descriptor that refers to that endpoint
*/

/*
** What is a blocking and non-blocking socket ?
** https://www.scottklement.com/rpg/socktut/nonblocking.html -> Good
*/

/*
** the poll() functions is an observer of a tab of pollfd that contains their current fds and the events that is happening
** at each one of them
** can we listen to multiple ports ? yes, theorically we can have multiplt "server_fds"
** and set the appropriate fd in fds[n].fd to the good one after an accept
*/
https://stackoverflow.com/questions/15560336/listen-to-multiple-ports-from-one-server
https://stackoverflow.com/questions/34447856/poll-and-read-resource-temporarily-unavailable

Functions used and explanations :

1.
/*
** Returns a socket descriptor (endpoint)
** only the protocol is specified (not yet the IP and PORT)
** A socket_descriptor represents the socket but is not a socket in itself (it is a binary that acts as an index to the socket)
*/
std::cout << port_number << std::endl;
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
	std::cerr << "socket error" << std::endl;
	return (1);
}

2.
/*
** Allows the application to reuse local address when the server restarts (CTRL-C + ./webserv) before waiting time expires
*/
if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
{
	printf("%s\n", strerror(errno));
	std::cerr << "setsockopt error" << std::endl;
	return (1);
}

3.
/*
** Gets a unique name for the socket
** bind() assigns the address specified by the second argument to the socket referred to by the file descriptor sever_fd
** it is here that the fd will be assigned an IP and PORT that he will afterwards listen to
*/
if (bind(server_fd, (sockaddr *)&sock_structs, sizeof(sockaddr_in)) < 0)
{
	printf("%s\n", strerror(errno));
	std::cerr << "bind error" << std::endl;
	return (1);
}

4.
/*
** Allows the server to accept incoming client connections
** listen() marks the socket referred by server_fd as a passive socket that will be used to accept incoming connection with accept
*/
if (listen(server_fd, 42) < 0) 
{ 
	printf("%s\n", strerror(errno));
	std::cerr << "listen error" << std::endl;
	return (1);
}

SO_REUSEADDR && SO_REUSEPORT
https://stackoverflow.com/questions/14388706/how-do-so-reuseaddr-and-so-reuseport-differ

HTTP 1.1 : RFC 7230 to 7237
https://datatracker.ietf.org/doc/html/rfc7230#section-1

