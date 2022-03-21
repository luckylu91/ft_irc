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

echo:
	@echo "SRCS = $(wildcard src/*.cpp)"
	@echo "HEADERS = $(wildcard include/*.hpp)"
	@echo "OBJS = $(SRCS:src/%.cpp=obj/%.o)"
	@echo "NAME = ft_irc"
	@echo "CFLAGS = -Iinclude -Wall -Wextra -Werror -std=c++98"

.PHONY: all clean fclean re
