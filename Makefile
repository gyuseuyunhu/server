NAME = webserv

SRCS = main.cpp \
			 util.cpp \

PARSE_SRCS = Parse.cpp \
						 ServerInfoStr.cpp \
	           Config.cpp \
						 ADirective.cpp \
						 HttpDirective.cpp \
						 ServerDirective.cpp \
						 LocationDirective.cpp \
						 HttpBlock.cpp \
						 ServerBlock.cpp \
						 LocationBlock.cpp \
						 BlockBuilder.cpp \

EVENT_SRCS = AEvent.cpp \
						 AcceptEvent.cpp \
						 ReadRequestEvent.cpp \
						 Request.cpp \
						 Response.cpp \
						 WriteEvent.cpp \
						 ReadFileEvent.cpp \
						 WriteCgiEvent.cpp \
						 ReadCgiEvent.cpp \

SERVER_SRCS = Kqueue.cpp \
							Server.cpp \
							ServerInfo.cpp \
							ServerManager.cpp \
						 HttpStatusInfos.cpp \

PARSE_DIR = parse
EVENT_DIR = event
SERVER_DIR = server

INC = include
INCLUDES = -I $(INC) -I $(INC)/$(PARSE_DIR) -I $(INC)/$(EVENT_DIR) -I $(INC)/$(SERVER_DIR) 

OBJS = $(SRCS:.cpp=.o)
PARSE_OBJS = $(PARSE_SRCS:.cpp=.o)
EVENT_OBJS = $(EVENT_SRCS:.cpp=.o)
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)

OBJ_DIR = obj
OBJ_DIRS = $(OBJ_DIR) \
					 $(OBJ_DIR)/$(PARSE_DIR) \
			     $(OBJ_DIR)/$(EVENT_DIR) \
					 $(OBJ_DIR)/$(SERVER_DIR) \

SRC_DIR = src

OBJS_FILES = $(addprefix $(OBJ_DIR)/, $(OBJS)) \
						 $(addprefix $(OBJ_DIR)/$(PARSE_DIR)/, $(PARSE_OBJS)) \
						 $(addprefix $(OBJ_DIR)/$(EVENT_DIR)/, $(EVENT_OBJS)) \
						 $(addprefix $(OBJ_DIR)/$(SERVER_DIR)/, $(SERVER_OBJS)) \

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all : $(NAME)

$(OBJ_DIRS) :
	@mkdir -p $(OBJ_DIRS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIRS)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

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