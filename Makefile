# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/12 14:40:50 by mcarneir          #+#    #+#              #
#    Updated: 2024/12/05 11:24:08 by mcarneir         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
SRCS_DIR = srcs
OBJ_DIR = bin
SRCS = $(wildcard $(SRCS_DIR)/*.cpp)
OBJ	= $(patsubst $(SRCS_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
RM = rm -rf
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INC = includes/

all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -I $(INC) -c $< -o $@
	
$(NAME): $(OBJ)
		@ $(CXX) $(OBJ) $(CXXFLAGS) -o $(NAME)

clean:
		@$(RM) $(OBJ) $(OBJ_DIR)
				
fclean: clean
		@$(RM) $(NAME)

re: fclean all
