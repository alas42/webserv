#include <arpa/inet.h> //   htons ... inet_addr
#include <sys/select.h> //  select
#include <poll.h> //        poll
#include <sys/epoll.h> //   epoll
#include <sys/socket.h> //  socket setsockopt accept listen send getsockname
//#include <sys/event.h>
#include <sys/types.h>
#include <iostream>     //  std::cout
#include <functional>   //  std::bind
#include <unistd.h>     //  fcntl
#include <fcntl.h>      //  fcntl

int main(int ac, char **av)
{
    (void)ac;
    (void)av;
    std::cout<< "First thing first, obtain a real connection between\n"
        << "the server and client." << std::endl;
    return (0);
}