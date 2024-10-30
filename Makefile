NAME = ircserv
CPP = g++
FLAGS = -Wall -Wextra -Werror -std=c++98
SRC = src/main.cpp \
		src/Server.cpp \
		src/Client.cpp \
		src/Channel.cpp \
		src/invite.cpp \
		src/join.cpp \
		src/kick.cpp \
		src/part.cpp \
		src/privmsg.cpp \
		src/quit.cpp \



OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CPP) $(FLAGS) -o $@ $(OBJ)
	@echo "$(NAME) compilation successful."

%.o: %.cpp
	@$(CPP) $(FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)
	@echo "Compiled objects have been removed."

fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME) Cleaned."

re: fclean all

.PHONY: all clean fclean re