# Sources options
TARGET ?= main
SIMILARITY_FILE ?= data/similarity.bin

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
CFLAGS := -std=c17 -Wall -Wextra -g -O3 -I $(PATH_INCLUDE)

# Linker options
LDFLAGS := -lm

# Colors options
GREEN = $(strip \033[0;32m)
DEFAULT = $(strip \033[0m)

# Commands
all: $(TARGET) unzip

run: $(TARGET)
	@printf "$(GREEN)Running $(TARGET):$(DEFAULT)\n"
	@./$(TARGET)

tests: $(TESTS)
	@printf "$(GREEN)Running tests...$(DEFAULT)\n"
	@for test in $(TESTS); do \
		printf "\n$(GREEN)Running $$test:$(DEFAULT)\n"; \
		./$$test; \
	done

doc::
	@mkdir -p $(PATH_DOC) && $(RM) -r $(PATH_DOC)*/
	@printf "$(GREEN)Generating documentation...$(DEFAULT)\n"
	@doxygen Doxyfile

clean:
	@printf "$(GREEN)Cleaning...$(DEFAULT)\n"
	$(RM) -r $(PATH_BUILD) $(PATH_EXE) $(PATH_OBJS) $(PATH_DEPS)
	$(RM) -r $(PATH_DOC)*/
	$(RM) $(TARGET) data/data.bin

unzip:
	@if [ -f $(SIMILARITY_FILE).zst ] && [ ! -f $(SIMILARITY_FILE) ]; then \
		echo -e "$(GREEN)Decompressing $(SIMILARITY_FILE).zst...$(DEFAULT)"; \
		unzstd -kf -T0 $(SIMILARITY_FILE).zst; \
	fi
	
# Rule for compiling a C source file
$(PATH_OBJS)%.o: $(PATH_SRC)%.c
	@mkdir -p $(PATH_OBJS)
	@printf "$(GREEN)Compiling $< $(DEFAULT)\n"
	$(CC) $(CFLAGS) -c $< -o $@
	@printf "\n"

# Rule for compiling a C test file
$(PATH_OBJS)%.o: $(PATH_TEST)%.c
	@mkdir -p $(PATH_OBJS)
	@$(CC) $(CFLAGS) -I $(PATH_UNITY) -c $< -o $@

# Rule for compiling main file
$(PATH_OBJS)$(TARGET).o: $(TARGET).c
	@mkdir -p $(PATH_OBJS)
	@printf "$(GREEN)Compiling $< $(DEFAULT)\n"
	$(CC) $(CFLAGS) -c $< -o $@
	@printf "\n"

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
	@printf "$(GREEN)Linking...$(DEFAULT)\n"
	$(CC) $^ -o $@ $(LDFLAGS)
	@printf "\n"

.PRECIOUS: $(PATH_DEPS)%.d
.PRECIOUS: $(PATH_OBJS)%.o
.PHONY: all clean
