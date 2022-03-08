NAME = webserv

HEADER_FILES = 

SOURCES_FILES =	main.cpp

OBJS = $(SOURCES_FILES:.cpp=.o)

CPPFLAGS = -Wall -Werror -Wextra 
CC = clang++ -std=c++98

all: $(NAME)

$(NAME): $(OBJS) $(HEADER_FILES)
	$(CC) $(CPPFLAGS) $(SOURCES_FILES) -o $(NAME)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
