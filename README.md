# webserv
This is when you finally understand why a URL starts with HTTP


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