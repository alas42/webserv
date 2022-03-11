NAME	=	webserv

CC		=	clang++

RM		=	rm -f

CFLAGS	=	-Wall -Wextra -Werror -g -fno-limit-debug-info

SRCS =	src/parsing_client_header.cpp \
	src/webserv.cpp \
 	src/process.cpp \
	src/parser.cpp \
	src/config/Config.cpp \
	src/server/Server.cpp \
	src/request/Request.cpp \
	src/execution_requests/Exec.cpp \
	src/launch/Launch.cpp \
	src/listen/Listen.cpp

INCLUDES	= -I src/webserv.hpp

OBJS	:=	${SRCS:.cpp=.o}

.PHONY: clean fclean re

all: ${NAME}

${NAME}: ${OBJS}
	$(CC) $(CFLAGS) -o $(NAME) $^

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) ${OBJS}

fclean: clean
	$(RM) ${NAME}

re: fclean all
