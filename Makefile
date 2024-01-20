# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tischmid <tischmid@student.42berlin.de>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/22 15:02:16 by tischmid          #+#    #+#              #
#    Updated: 2024/01/20 14:59:27 by tosuman          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Makefile for Linux systems
# libft get_next_line BUFFER_SIZE kept at 1 because on at least on occasion
# I had still reachable leaks since the static buffer was not cleared (which
# isn't really a problem save for the perfectionist). For some reason
# it started to work again with different buffer sizes, but to play it safe,
# I'll keep it at 1.

NAME         = pipex
LIBFT        = libft.a
LIBFT_       = $(patsubst lib%,%,$(patsubst %.a,%,$(LIBFT)))

_SRC         = error_handling.c init.c pipex.c utils.c
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
CFLAGS       = -O3 -Wall -Wextra -Werror \
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

bonus: $(NAME)

clean:
	$(MAKE) -C $(LIBFT_DIR) $@
	$(RM) $(OBJ)
	$(RM) -rf $(OBJDIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) $@
	$(RM) $(NAME)

re: fclean all

.PHONY: re fclean clean all bonus
