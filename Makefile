# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nguiard <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/25 15:46:23 by nguiard           #+#    #+#              #
#    Updated: 2024/11/13 10:31:00 by nguiard          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC	=	$(addsuffix .c,		\
		$(addprefix srcs/,	\
			main			\
			inject			\
			utils			\
			key_generation	\
		))

NAME	= woody_woodpacker

SHELL	= /bin/zsh

OBJ		= ${SRC:srcs/%.c=.obj/%.o}

CC		= gcc

INCLUDE = -Iincludes/

CFLAGS	= -Wall -Werror -Wextra -pipe ${INCLUDE} -g3 #-fsanitize=address

all: ${NAME} 

.obj/%.o: srcs/%.c
	@${CC} ${CFLAGS} -c $< -o ${<:srcs/%.c=.obj/%.o}

.obj/%.o: srcs/%.s
	@nasm -f elf64 -o ${<:srcs/%.s=.obj/%.o} $<
	@chmod o+wr ${<:srcs/%.s=.obj/%.o}

${NAME}: ${OBJ}
	@echo "Compiling ${NAME}"
	@${CC} ${OBJ} ${CFLAGS} -o ${NAME}
	@echo "${NAME} compiled"


clean:
	@rm -rf ${OBJ}


fclean:
	@rm -rf ${OBJ} ${NAME}


re: fclean all


.PHONY: all clean fclean re debug
