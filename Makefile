# Sources options
TARGET ?= prog

PATH_SOURCE ?= src/
PATH_BUILD ?= build/
PATH_OBJS ?= build/objs/
PATH_UNITY ?= unity/src/
PATH_INCLUDE ?= include/
PATH_TEST ?= test/

SOURCES := $(wildcard $(PATH_SOURCE)*.c)
OBJECTS := $(SOURCES:$(PATH_SOURCE)%.c=$(PATH_OBJS)%.o)
SOURCES_TEST := $(wildcard $(PATH_TEST)*.c)
TESTS := $(SOURCES_TEST:$(PATH_TEST)%.c=$(PATH_BUILD)$(PATH_TEST)%)

# Compiler options
CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -g -I $(PATH_INCLUDE)

# Linker options
LDFLAGS := -lm

# Colors options
GREEN = $(strip \033[0;32m)
DEFAULT = $(strip \033[0m)

# Commands
.PHONY: all clean
all: $(TARGET) run

# Rule for building a C source file
$(PATH_OBJS)%.o: $(PATH_SOURCE)%.c 
	@mkdir -p $(dir $@)
	@echo -e "\n$(GREEN)Compiling $< $(DEFAULT)"
	$(CC) $(CFLAGS) -c $< -o $@

# Build Unity
$(PATH_BUILD)unity/%.o: $(PATH_UNITY)%.c $(PATH_UNITY)%.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(PATH_UNITY) -c $< -o $@

# Rule for building a C test file
$(PATH_OBJS)%.o: $(PATH_TEST)%.c $(PATH_BUILD)unity/unity.o
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -I $(PATH_UNITY) -c $< -o $@

$(PATH_BUILD)$(PATH_TEST)test_%: $(PATH_OBJS)%.o $(PATH_OBJS)test_%.o $(PATH_BUILD)unity/unity.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@ $(LDFLAGS)

$(TARGET): $(OBJECTS)
	@echo -e "\n$(GREEN)Linking...$(DEFAULT)"
	$(CC) $^ -o $@ $(LDFLAGS)

tests: $(TESTS)
	@echo -e "\n$(GREEN)Running all tests:$(DEFAULT)"
	./$(PATH_BUILD)$(PATH_TEST)*

run:
	@echo -e "\n$(GREEN)Running $(TARGET):$(DEFAULT)"
	@./$(TARGET)

clean:
	@echo -e "\n$(GREEN)Cleaning...$(DEFAULT)"
	$(RM) -r $(PATH_BUILD) $(TARGET)

