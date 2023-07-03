# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: inovomli <inovomli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/13 16:18:54 by sbritani          #+#    #+#              #
#    Updated: 2023/07/01 20:40:13 by inovomli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++

SRCS = main.cpp Channel.cpp Message.cpp Server.cpp User.cpp bot.cpp

NAME =  ircserv

FLAGS = -g #-Wall -Wextra -Werror -std=c++98 -fsanitize=address -g 

OBJ	= $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(FLAGS) -o $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -c $^ -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
