NAME = ircserv
CPP = g++
FLAGS = -Wall -Wextra -Werror -std=c++98
SRC = main.cpp \
		Server.cpp


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