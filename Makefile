## Short makefile to compile my C chess engine

## Where our implementation is located (don't change)
SRCDIR = src
## List all files you want to be part of your engine below
## (Okay to add more files)
SOURCES = \
  $(SRCDIR)/engine.c \
  $(SRCDIR)/fen.c \
  $(SRCDIR)/moves.c \
  $(SRCDIR)/board.c

## You SHOULD NOT modify the parameters below

## Compiler to use by default
CC = gcc

## Compiler flags
CFLAGS = -Wall -Wextra -Werror -pedantic -Iinclude

## Where to put the object files
BINDIR ?= build

## Object filenames derived from source filenames
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)

# The name of the binary (executable) file
TARGET ?= engine

## Optional target: useful for running the website
WEB_TARGET ?= web/engine.wasm

## Emscripten compiler
EMCC = emcc

## Emscripten flags
EMCC_FLAGS = -s WASM=1 -s EXPORTED_FUNCTIONS='["_choose_move"]' --no-entry -O3 -Iinclude

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

## Format the source code using clang-format
CLANG_FORMAT = clang-format
CLANG_FORMAT_OPTS = -i

.PHONY: format
format:
	@echo "Running clang-format on source files..."
	@which $(CLANG_FORMAT) > /dev/null || (echo "clang-format is not installed. Please install it first." && exit 1)
	@find $(SRCDIR) -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" \) -exec $(CLANG_FORMAT) $(CLANG_FORMAT_OPTS) {} +
