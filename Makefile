# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nguiard <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/25 15:46:23 by nguiard           #+#    #+#              #
#    Updated: 2024/11/14 12:29:27 by nguiard          ###   ########.fr        #
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
	@rm -rf *.woody


test:
	@make -C resources
	@echo -e "\n\033[32mTests:\n\033[0m"
	@echo -e "\033[1mNormal\033[0m executable\n"
	@./${NAME} resources/normal normal.woody
	@echo -e "\n\033[1m32bits\033[0m executable\n"
	@./${NAME} resources/32bits 32bits.woody
	@echo -e "\n\033[1mno-pie\033[0m executable\n"
	@./${NAME} resources/nopie  nopie.woody
	@echo -e "\n\033[1mAssembly\033[0m executable\n"
	@./${NAME} resources/asm    asm.woody


re: fclean all


.PHONY: all clean fclean re test
