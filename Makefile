NAME = webserv

SRCS = main.cpp \
			 Config.cpp \
			 ADirective.cpp \
			 HttpBlock.cpp \
			 LocationBlock.cpp \
			 HttpDirective.cpp \
			 ServerDirective.cpp \
			 LocationDirective.cpp \
			 Server.cpp \
			 ServerBlock.cpp \
			 util.cpp \
			 BlockBuilder.cpp \
			 Parse.cpp \
			 ServerInfo.cpp \
			 ServerInfoStr.cpp \
			 ServerManager.cpp \
			 Accept.cpp \
			 Read.cpp \
			 AEvent.cpp \
			 Kqueue.cpp \
		   Request.cpp \

INC = include

OBJS = $(SRCS:.cpp=.o)

OBJ_DIR = obj
SRC_DIR = src

OBJS_FILES = $(addprefix $(OBJ_DIR)/, $(OBJS))

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all : $(NAME)

$(OBJ_DIR) :
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -I $(INC)

$(NAME) : $(OBJS_FILES)
	@$(CXX) $(CXXFLAGS) $(OBJS_FILES) -o $(NAME)
	@echo "\033[32mmake $(NAME)\033[0m"

clean :
	@rm -rf $(OBJ_DIR)
	@echo "\033[33mremove obj\033[0m"
.PHONY : clean

fclean : clean
	@rm -rf $(NAME)
	@echo "\033[33mremove $(NAME)\033[0m"
.PHONY : fclean

re : fclean
	@make all
.PHONY : re