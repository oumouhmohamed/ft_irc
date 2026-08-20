CPP = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = main.cpp channel.cpp client.cpp parsing.cpp server.cpp server_channel2.cpp server_cmds.cpp server_cmds_channel.cpp mode.cpp
OBJ = $(SRC:.cpp=.o)

NAME = ircserv

all : $(NAME)

$(NAME): $(OBJ)
		$(CPP) $(CPPFLAGS) $(OBJ) -o $(NAME)

%.o:%.cpp channel.hpp client.hpp parsing.hpp server.hpp 
		$(CPP) $(CPPFLAGS) -c $<  -o $@
clean:
	rm -f $(OBJ)

fclean:clean
	rm -f $(NAME)

re: fclean all
