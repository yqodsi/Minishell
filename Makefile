NAME = minishell

CC = gcc

CFLAGS = -Wall -Wextra -Werror -I includes/ -I libft/includes/


HEADER = minishell.h

SRCS_DIR	=	./srcs/
OBJ_DIR		=	./objs/
INC_DIR		=	./includes/


SRC = bin.c 
SRC += builtin.c 
SRC += cd.c 
SRC += echo.c 
SRC += env.c 
SRC += exec.c 
SRC += exit.c 
SRC += expansions.c 
SRC += export.c 
SRC += fd.c 
SRC += get_env.c 
SRC += minishell.c 
SRC += parse.c 
SRC += pwd.c 
SRC += redir.c 
SRC += token.c 
SRC += tokens.c 
SRC += tools1.c 
SRC += type.c 
SRC += unset.c 
SRC += signals.c 


OBJ			=	$(addprefix $(OBJ_DIR),$(SRC:.c=.o))

INC			=	$(addprefix -I,$(INC_DIR))

LIBFT		=	./libft/libft.a

RM			=	/bin/rm -f
RM_DIR		=	/bin/rm -rf

$(OBJ_DIR)%.o:$(SRCS_DIR)%.c $(INC_DIR)*.h
	$(CC)  $(INC) -c $< -o $@


all:
	@mkdir -p $(OBJ_DIR)
	@$(MAKE) $(NAME) --no-print-directory
	@echo " \e[32m\e[1mEveryThing is Good!!"

make_libft:
	@make -C libft/

$(NAME): $(OBJ) $(INC_DIR) make_libft 
	@$(CC)  $(OBJ) $(LIBFT) -I  $(INC) -o $(NAME)

clean:
	@$(RM_DIR) $(OBJ_DIR)
	@$(MAKE) clean -C libft/ --no-print-directory
	@echo " \e[32m\e[1mAll clean!!"

fclean: clean
	@make fclean -C libft/ --no-print-directory
	@rm -f $(NAME)

re: fclean all

test: all
	./minishell

norm:
	norminette $(SRC) includes/$(HEADER)