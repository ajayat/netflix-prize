# Sources options
TARGET ?= netflix_prize

PATH_SOURCE ?= src/
PATH_BUILD ?= build/
PATH_OBJS ?= build/objs/
PATH_TEST_EXE ?= build/tests/
PATH_UNITY ?= modules/unity/src/
PATH_INCLUDE ?= include/
PATH_TEST ?= test/
PATH_DOC ?= doc/
DATA_BIN ?= data/data.bin

SOURCES := $(wildcard $(PATH_SOURCE)*.c)
OBJECTS := $(SOURCES:$(PATH_SOURCE)%.c=$(PATH_OBJS)%.o)
SOURCES_TEST := $(wildcard $(PATH_TEST)*.c)
TESTS := $(SOURCES_TEST:$(PATH_TEST)%.c=$(PATH_TEST_EXE)%)

# Compiler options
CC := gcc
CFLAGS := -std=c2x -Wall -Wextra -g -O3 -I $(PATH_INCLUDE)

# Linker options
LDFLAGS := -lm

# Colors options
GREEN = $(strip \033[0;32m)
DEFAULT = $(strip \033[0m)

# Commands
.PHONY: all clean
all: $(TARGET)

# Rule for building a C source file
$(PATH_OBJS)%.o: $(PATH_SOURCE)%.c 
	@mkdir -p $(dir $@)
	@echo -e "$(GREEN)Compiling $< $(DEFAULT)"
	$(CC) $(CFLAGS) -c $< -o $@
	@echo

# Build Unity
$(PATH_BUILD)unity/%.o: $(PATH_UNITY)%.c $(PATH_UNITY)%.h
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -I $(PATH_UNITY) -c $< -o $@

# Rule for building a C test file
$(PATH_OBJS)%.o: $(PATH_TEST)%.c $(PATH_BUILD)unity/unity.o
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -I $(PATH_UNITY) -c $< -o $@

$(PATH_TEST_EXE)test_%: $(PATH_OBJS)%.o $(PATH_OBJS)test_%.o $(PATH_BUILD)unity/unity.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@ $(LDFLAGS)

# Rule for building the final executable
$(TARGET): $(OBJECTS)
	@echo -e "$(GREEN)Linking...$(DEFAULT)"
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo

tests: $(TESTS)
	@echo -e "$(GREEN)Running tests...$(DEFAULT)"
	@for test in $(TESTS); do \
		echo -e "$(GREEN)Running $$test:$(DEFAULT)"; \
		./$$test; \
		@echo; \
	done

doc:
	@mkdir -p $(PATH_DOC)
	@echo -e "$(GREEN)Generating documentation...$(DEFAULT)"
	@doxygen Doxyfile

run: $(TARGET) unzip
	@echo -e "$(GREEN)Running $(TARGET):$(DEFAULT)"
	@./$(TARGET)

zip:
	@if [ -f $(DATA_BIN) ]; then \
		echo -e "$(GREEN)Compressing $(DATA_BIN)...$(DEFAULT)"; \
		zstd -kf -T0 -5 $(DATA_BIN); \
		echo -e Done; \
	fi

unzip:
	@if [ -f $(DATA_BIN).zst ] && [ ! -f $(DATA_BIN) ]; then \
		echo -e "$(GREEN)Decompressing $(DATA_BIN).xz...$(DEFAULT)"; \
		unzstd -kf -T0 $(DATA_BIN).zst; \
	fi

clean:
	@echo -e "$(GREEN)Cleaning...$(DEFAULT)"
	$(RM) -r $(PATH_BUILD) $(PATH_TEST_EXE) $(PATH_OBJS) $(PATH_DOC) $(DATA_BIN) $(TARGET) 
