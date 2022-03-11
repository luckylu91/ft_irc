SRCS = $(wildcard src/*.cpp)
HEADERS = $(wildcard include/*.cpp)
NAME = ft_irc
CFLAGS = -Iinclude -Wall -Wextra -Werror -std=c++98


all: $(NAME)

$(NAME): $(OBJS) | $(HEADERS)
	g++ -o $@ $^ $(CFLAGS)


