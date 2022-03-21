SRCS = $(wildcard src/*.cpp)
HEADERS = $(wildcard include/*.hpp)
OBJS = $(SRCS:src/%.cpp=obj/%.o)
NAME = ft_irc
CFLAGS = -Iinclude -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJS) | $(HEADERS)
	g++ -o $@ $^ $(CFLAGS)

obj/%.o: src/%.cpp | obj
	g++ -c -o $@ $^ $(CFLAGS)

obj:
	mkdir obj/

clean:
	rm -rf obj/

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
