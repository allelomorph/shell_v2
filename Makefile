# -*- MakeFile -*-
# Holberton School Shell v2 - project 327

CC = gcc
SRC = hsh.c \
	builtins.c errors.c lexing.c kv_lists.c \
	scripts.c shellLoop-subr.c st_lists.c \
	string-utils1.c string-utils2.c _which.c
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
