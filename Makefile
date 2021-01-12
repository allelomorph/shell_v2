# -*- MakeFile -*-
# Holberton School Shell v2 - project 327

CC = gcc
SRC = hsh.c \
	builtin_cd.c \
	builtins.c \
	cmd_lists.c \
	errors1.c \
	errors2.c \
	execution.c \
	kv_lists1.c \
	kv_lists2.c \
	lexing.c \
	redirects1.c \
	redirects2.c \
	redirects3.c \
	shell_scripts.c \
	shell_loop.c \
	st_lists.c \
	string_utils1.c \
	string_utils2.c \
	string_utils3.c \
	_which.c
OBJ = $(SRC:.c=.o)
NAME = hsh
CFLAGS = -Wall -Werror -Wextra -pedantic

.PHONY: clean oclean fclean re

all: holberton.h $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

clean:
	$(RM) *~ $(NAME)

oclean:
	$(RM) $(OBJ)

fclean: clean oclean

re: oclean all
