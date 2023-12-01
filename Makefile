# Sources options
TARGET ?= main
DATA_FILE ?= data/data.bin

# Directories
PATH_SRC ?= src/
PATH_BUILD ?= build/
PATH_OBJS ?= build/objs/
PATH_EXE ?= build/bin/
PATH_UNITY ?= unity/src/
PATH_INCLUDE ?= include/
PATH_TEST ?= test/
PATH_DOC ?= doc/

# Sources files and objects
SRCS := $(wildcard $(PATH_SRC)*.c)
OBJECTS := $(SRCS:$(PATH_SRC)%.c=$(PATH_OBJS)%.o)

# Test files and executables
SRCS_TEST := $(wildcard $(PATH_TEST)*.c)
TESTS := $(SRCS_TEST:$(PATH_TEST)%.c=$(PATH_EXE)%)

# Compiler options
CC := gcc
CFLAGS := -std=c2x -Wall -Wextra -g -O3 -I $(PATH_INCLUDE)

# Linker options
LDFLAGS := -lm

# Colors options
GREEN = $(strip \033[0;32m)
DEFAULT = $(strip \033[0m)

# Commands
all: $(TARGET)

run: $(TARGET) unzip
	@echo -e "$(GREEN)Running $(TARGET):$(DEFAULT)"
	@./$(TARGET)

tests: $(TESTS)
	@echo -e "$(GREEN)Running tests...$(DEFAULT)"
	@for test in $(TESTS); do \
		echo -e "\n$(GREEN)Running $$test:$(DEFAULT)"; \
		./$$test; \
	done

doc::
	@mkdir -p $(PATH_DOC)
	@echo -e "$(GREEN)Generating documentation...$(DEFAULT)"
	@doxygen Doxyfile

zip:
	@echo -e "$(GREEN)Compressing $(DATA_FILE)...$(DEFAULT)";
	zstd -kf -T0 -5 $(DATA_FILE);

unzip:
	@if [ -f $(DATA_FILE).zst ] && [ ! -f $(DATA_FILE) ]; then \
		echo -e "$(GREEN)Decompressing $(DATA_FILE).xz...$(DEFAULT)"; \
		unzstd -kf -T0 $(DATA_FILE).zst; \
	fi

clean:
	@echo -e "$(GREEN)Cleaning...$(DEFAULT)"
	$(RM) -r $(PATH_BUILD) $(PATH_EXE) $(PATH_OBJS) $(PATH_DEPS) $(PATH_DOC) 
	$(RM) $(DATA_FILE) $(TARGET)

# Rule for compiling a C source file
$(PATH_OBJS)%.o: $(PATH_SRC)%.c
	@mkdir -p $(PATH_OBJS)
	@echo -e "$(GREEN)Compiling $< $(DEFAULT)"
	$(CC) $(CFLAGS) -c $< -o $@
	@echo

# Rule for compiling a C test file
$(PATH_OBJS)%.o: $(PATH_TEST)%.c
	@mkdir -p $(PATH_OBJS)
	@$(CC) $(CFLAGS) -I $(PATH_UNITY) -c $< -o $@

# Rule for compiling main file
$(PATH_OBJS)$(TARGET).o: $(TARGET).c
	@mkdir -p $(PATH_OBJS)
	@echo -e "$(GREEN)Compiling $< $(DEFAULT)"
	$(CC) $(CFLAGS) -c $< -o $@
	@echo

# Build Unity
$(PATH_OBJS)unity.o: $(PATH_UNITY)unity.c $(PATH_UNITY)unity.h
	@mkdir -p $(PATH_OBJS)
	@$(CC) $(CFLAGS) -I $(PATH_UNITY) -c $< -o $@

# Rule for building a C test executable
$(PATH_EXE)test_%: $(PATH_OBJS)test_%.o $(OBJECTS) $(PATH_OBJS)unity.o
	@mkdir -p $(PATH_EXE)
	@$(CC) $(CFLAGS) -I $(PATH_UNITY) $^ -o $@ $(LDFLAGS)

# Rule for building the final executable
$(TARGET): $(OBJECTS) $(PATH_OBJS)$(TARGET).o
	@mkdir -p $(PATH_EXE)
	@echo -e "$(GREEN)Linking...$(DEFAULT)"
	$(CC) $^ -o $@ $(LDFLAGS)

.PRECIOUS: $(PATH_DEPS)%.d
.PRECIOUS: $(PATH_OBJS)%.o
.PHONY: all clean
