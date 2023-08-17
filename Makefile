# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sgodin <sgodin@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/13 14:39:47 by sgodin            #+#    #+#              #
#    Updated: 2023/08/17 15:38:46 by sgodin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS =	main.c\
		lexer.c\
		parser.c\ 
		executer.c\
			env_utils.c\
				builtins/builtin.c\
				builtins/builtin_tools.c\
				builtins/cd.c\
				builtins/echo.c\
				builtins/exit.c\
				builtins/export.c\
				builtins/export2.c\
				builtins/unset.c\

OBJS_DIR = ./objects/
OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.c=.o))


SHELL = bash
NAME = minishell
GCC = gcc -Wall -Wextra -Werror -g #-fsanitize=address

RED = \033[1;31m
GREEN = \033[1;32m
BLUE = \033[1;34m
RESET = \033[0m

all: ${NAME}

${NAME}: ${OBJS}
	@echo -ne "${BLUE}Building libft...${RESET}"
	@make -sC ./libft
	@${GCC} ${OBJS} -o ${NAME} -lreadline -L./libft -lft && echo -ne "\r${BLUE}Compiling ${NAME} ${GREEN}Done${RESET}\n" || echo -ne "\r${BLUE}Compiling ${NAME} ${RED}Error${RESET}\n"

$(OBJS_DIR)%.o: %.c
	@mkdir -p $(dir $@)
	@gcc $(FLAGS) -c $< -o $@

clean:
	@echo -ne "${RED}Cleaning...${RESET}"
	@make clean -sC ./libft
	@rm -rf ${OBJS} && echo -ne "\r${RED}Cleaning ${GREEN}Done${RESET}\n" || echo -ne "\r${RED}Cleaning ${RED}Error${RESET}\n"
	@rm -rf $(OBJS_DIR)

fclean: clean
	@make fclean -sC ./libft
	@rm -rf $(NAME)

re: fclean all

.c.o:
	@echo -ne "${BLUE}Compiling $<${RESET}"
	@gcc -Wall -Wextra -Werror -c $< -o ${<:.c=.o} && echo -ne "\r\033[K"

.PHONY: all clean fclean re .c.o bonus