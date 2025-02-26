## Short makefile to compile my C chess engine

## Where our implementation is located (don't change)
SRCDIR = src
TESTDIR = test
LIBDIR = libs/unity

## List all files you want to be part of your engine below
## (Okay to add more files)
SOURCES = \
  $(SRCDIR)/engine.c \
  $(SRCDIR)/fen.c \
  $(SRCDIR)/moves.c \
  $(SRCDIR)/print.c \
  $(SRCDIR)/board.c \
  $(SRCDIR)/eval.c \
  $(SRCDIR)/search.c \
  $(SRCDIR)/zobrist.c \
  $(SRCDIR)/transposition.c

## You SHOULD NOT modify the parameters below

## Compiler to use by default
CC = gcc

## Compiler flags
CFLAGS = -Wall -Wextra -Werror -pedantic -Iinclude -O3

## Where to put the object files
BINDIR ?= build

## Object filenames derived from source filenames
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)

## The name of the binary (executable) file
TARGET ?= engine

## Optional target: useful for running the website
WEB_TARGET ?= web/engine.wasm

## Emscripten compiler
EMCC = emcc

## Emscripten flags
EMCC_FLAGS = -s WASM=1 -s EXPORTED_FUNCTIONS='["_choose_move"]' -s STACK_SIZE=8388608 --no-entry -O3 -Iinclude

## Create the build directory if it doesn't exist
$(BINDIR):
	mkdir -p $(BINDIR)

## Compile each object file
$(BINDIR)/%.o: $(SRCDIR)/%.c $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

## Compile the final binary
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

## Only build the binary by default
all: $(TARGET)

## Optional target: build the web target
$(WEB_TARGET): $(SOURCES)
	emcc $(EMCC_FLAGS) $^ -o $@

## Start python3 web server to run the website for the folder web
.PHONY: run
run: $(WEB_TARGET)
	python3 -m http.server --directory web

## Clean up the build directory
.PHONY: clean
clean:
	rm -rf $(BINDIR) $(TARGET) $(WEB_TARGET)

## Unit Testing Configuration

## Test directory and Unity library location
TESTDIR = tests
LIBDIR = libs/unity

## List all test files
TEST_SOURCES = \
  $(TESTDIR)/perft.c

## Test object filenames
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.c=$(BINDIR)/%.o)

## Name of the test binary
TEST_BIN = test_runner

## Ensure Unity is included in the compilation
CFLAGS += -I$(LIBDIR)

## Compile each test object file
$(BINDIR)/%.o: $(TESTDIR)/%.c $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

## Build the test executable separately
$(TEST_BIN): $(TEST_OBJECTS) $(filter-out $(BINDIR)/engine.o, $(OBJECTS)) $(LIBDIR)/unity.c
	$(CC) $(CFLAGS) $^ -o $@

## Run the tests
.PHONY: test
test: $(TEST_BIN)
	./$(TEST_BIN)
