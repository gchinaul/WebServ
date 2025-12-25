NAME := webserv

SOURCES := ./src
INCLUDE := ./include
OBJECTS := ./bin

EOC := \033[0m
GREEN := \033[32m
CYAN := \033[36m
BLUE := \033[34m
YELLOW := \033[33m
RED := \033[31m

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -g3 -O1 -I$(INCLUDE)
LDFLAGS :=

SRCS := $(shell find $(SOURCES) -name '*.cpp' -type f)
HEADERS := $(shell find $(INCLUDE) -name '*.hpp' -type f)
OBJS := $(patsubst $(SOURCES)/%.cpp,$(OBJECTS)/%.o,$(SRCS))
OBJ_SUBDIRS := $(sort $(dir $(OBJS)))

TOTAL_FILES := $(words $(SRCS))
COMPILED := 0

all: banner $(NAME)

banner:
	@echo "$(BLUE)"
	@echo "  ╔═══════════════════════════════════════════════╗"
	@echo "  ║                                               ║"
	@echo "  ║           🌐  W E B S E R V  🌐               ║"
	@echo "  ║                                               ║"
	@echo "  ║            42 School Webserver                ║"
	@echo "  ║                                               ║"
	@echo "  ╚═══════════════════════════════════════════════╝"
	@echo "$(EOC)"

$(OBJS): | $(OBJ_SUBDIRS)

$(NAME): $(OBJS)
	@echo "$(GREEN)Compilation ${NAME}.$(EOC)"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN) 🤹 $(NAME) compiled success!$(EOC)"

${OBJECTS}/%.o: ${SOURCES}/%.cpp $(HEADERS)
	@echo "$(YELLOW) Compilation $(notdir $<) $(EOC)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_SUBDIRS):
	@mkdir -p $(OBJ_SUBDIRS)

clean:
	@echo "$(CYAN)Nettoyage des fichiers objets.$(EOC)"
	@rm -rf ${OBJECTS}

fclean: clean
	@echo "$(RED)Suppression de l'exécutable.$(EOC)"
	@rm -f $(NAME)

re: fclean all

debug: CFFFLAGS += -DDEBUG
debug: re

sanitize: CXXFLAGS += -fsanitize=address -fsanitize=undefined
sanitize: LDFLAGS += -fsanitize=address -fsanitize=undefined
sanitize: re

help:
	@echo "$(BLUE)=== 🕺 WEBSERV MAKEFILE BABY 🕺 ===$(EOC)"
	@echo "$(GREEN)all$(EOC)		- Compile the project"
	@echo "$(GREEN)clean$(EOC)		- Remove object files"
	@echo "$(GREEN)fclean$(EOC)		- Remove object files and executable"
	@echo "$(GREEN)re$(EOC)		- Rebuild everything"
	@echo "$(GREEN)debug$(EOC)		- Compile with debug flags"
	@echo "$(GREEN)sanitize$(EOC)	- Compile with sanitizers"
	@echo "$(GREEN)help$(EOC)		- Show this help"

.PHONY: all clean fclean re debug sanitize help