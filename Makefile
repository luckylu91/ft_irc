SRCS = $(wildcard src/*.cpp) $(wildcard src/server/*.cpp)
HEADERS = $(wildcard include/*.hpp)
OBJS = $(SRCS:src/%.cpp=obj/%.o)
NAME = ircserv
CFLAGS = -Iinclude -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(HEADERS)
$(NAME): $(OBJS)
	g++ -o $@ $(OBJS) $(CFLAGS)

$(OBJS): $(HEADERS)
obj/%.o: src/%.cpp | obj obj/server
	g++ -c -o $@ $< $(CFLAGS)

obj obj/server:
	mkdir -p obj/server

clean:
	rm -rf obj/

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
