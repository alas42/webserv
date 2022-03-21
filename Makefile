NAME	=	webserv

CC		=	clang++

RM		=	rm -f

# CFLAGS	=	-Wall -Wextra -Werror -std=c++98
CFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g -fno-limit-debug-info

SRCS =	src/webserv.cpp \
	src/config/Config.cpp \
	src/server/Server.cpp \
	src/request/Request.cpp \
	src/response/Response.cpp \
	src/execution_requests/Exec.cpp \
	src/listen/Listen.cpp \
	src/client/Client.cpp \
	# src/parser.cpp \

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
