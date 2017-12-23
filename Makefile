NAME_S = serveur
NAME_C = client

SRC = src/

DIR_H = inc/
DIR_S = $(SRC)serv/
DIR_C = $(SRC)cli/

OBJ = obj/
DIR_OS = $(OBJ)serv/
DIR_OC = $(OBJ)cli/

FILE_SC = main.c
FILE_CC = main.c

FILE_SO = $(FILE_SC:.c=.o)
FILE_CO = $(FILE_CC:.c=.o)

SRC_SC := $(addprefix $(DIR_S), $(FILE_SC))
SRC_CC := $(addprefix $(DIR_C), $(FILE_CC))

SRC_SO := $(addprefix $(DIR_OS), $(FILE_SO))
SRC_CO := $(addprefix $(DIR_OC), $(FILE_CO))

FLAGS = -Wall -Wextra -Werror # -g

LIBFT = ./libft/libft.a

all: $(NAME_S)

$(OBJ):
	mkdir -p $@
$(DIR_OS):
	mkdir -p $@
$(DIR_OC):
	mkdir -p $@

obj/serv/%.o: src/serv/%.c
	gcc $(FLAGS) -o $@ -c $< -I $(DIR_H) # -ggdb

obj/cli/%.o: src/cli/%.c
	gcc $(FLAGS) -o $@ -c $< -I $(DIR_H)
	 # -ggdb

$(NAME_S): $(OBJ) $(DIR_OS) $(DIR_OC) $(SRC_SO) $(SRC_CO) $(LIBFT)
		gcc -o $(NAME_S) $(SRC_SO) $(FLAGS) -L libft/ -lft
		gcc -o $(NAME_C) $(SRC_CO) $(FLAGS) -L libft/ -lft

$(LIBFT):
	make -C libft/

clean:
	/bin/rm -rf $(OBJ)

fclean: clean
	/bin/rm -f $(NAME_S)
	/bin/rm -f $(NAME_C)
	make fclean -C libft/

re: fclean all

.PHONY: clean fclean all re
