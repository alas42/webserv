NAME	=	webserv

CC		=	c++

RM		=	rm -f

CFLAGS	=	-Wall -Wextra -Werror -std=c++98

SRCS	=	src/webserv.cpp \
			src/config/Config.cpp \
			src/server/Server.cpp \
			src/request/Request.cpp \
			src/response/Response.cpp \
			src/client/Client.cpp \
			src/utils/utils.cpp \
			src/cgi/Cgi.cpp \
			src/request/Parser.cpp


INCLUDES	=	inc/webserv.hpp \
				src/config/Config.hpp \
				src/server/Server.hpp \
				src/request/Request.hpp \
				src/response/Response.hpp \
				src/client/Client.hpp \
				src/cgi/Cgi.hpp \
				src/request/Parser.hpp

OBJS	=	${SRCS:.cpp=.o}

.PHONY: clean fclean re

all: ${NAME}

${NAME}: ${OBJS} $(INCLUDES)
	$(CC) $(CFLAGS) $(SRCS) -o $(NAME)
	rm -f request_POST_0 
	rm -f abc

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) ${OBJS}

fclean: clean
	$(RM) ${NAME}

re: fclean all
