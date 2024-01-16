# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tischmid <tischmid@student.42berlin.de>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/22 15:02:16 by tischmid          #+#    #+#              #
#    Updated: 2023/12/07 20:07:23 by tosuman          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Makefile for Linux systems

NAME         = pipex
LIBFT        = libft.a
LIBFT_       = $(patsubst lib%,%,$(patsubst %.a,%,$(LIBFT)))

_SRC         = pipex.c
_OBJ         = $(_SRC:.c=.o)
_HEADERS	 = pipex.h
LIBFT_DIR    = ./libft

SRCDIR       = src
OBJDIR       = obj
INCLUDEDIR   = include
SRC          = $(addprefix $(SRCDIR)/,$(_SRC))
OBJ          = $(addprefix $(OBJDIR)/,$(_OBJ))
INCLUDE      = $(addprefix $(INCLUDEDIR)/,$(_HEADERS))

CC           = cc
CFLAGS       = -g3 -O0 -Wall -Wextra -Werror \
		       -std=c89 -pedantic -Wconversion
CPPFLAGS     = -I$(LIBFT_DIR) -I$(INCLUDEDIR)
LDFLAGS      = -L$(LIBFT_DIR)
LDLIBS       = -l$(LIBFT_)

all: $(NAME)

$(NAME): $(LIBFT_DIR)/$(LIBFT) $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LDLIBS)
	$(MAKE)

$(LIBFT_DIR)/$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ): $(INCLUDE) | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	$(MAKE) -C $(LIBFT_DIR) $@
	$(RM) $(OBJ)
	$(RM) -rf $(OBJDIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) $@
	$(RM) $(NAME)

re: fclean all

.PHONY: re fclean clean all
