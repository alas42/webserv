NAME	=	webserv

CC		=	clang++

RM		=	rm -f

CFLAGS	=	-Wall -Wextra -Werror -std=c++98

SRCS	=	main.cpp

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
