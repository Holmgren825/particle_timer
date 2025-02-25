# Makefile for compiling a C program using raylib
# Default target: all (builds for all platforms)

APP_NAME = particle_timer
SRC = main.c
BUILD_DIR = build
OBJ = $(addprefix $(BUILD_DIR)/,$(SRC:.c=.o))

# Compiler and Common Flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Default target
all: linux macos

# --- Linux Configuration ---
LINUX_EXE = $(BUILD_DIR)/$(APP_NAME)
LINUX_CFLAGS = $(CFLAGS) $(shell pkg-config --cflags raylib)
LINUX_LDFLAGS = $(shell pkg-config --libs raylib) -lm

linux: $(BUILD_DIR) $(OBJ)
	$(CC) $(LINUX_CFLAGS) $(OBJ) -o $(LINUX_EXE) $(LINUX_LDFLAGS)

# --- macOS Configuration ---
MACOS_EXE = $(BUILD_DIR)/$(APP_NAME)
MACOS_CFLAGS = $(CFLAGS) -I/opt/homebrew/include
MACOS_LDFLAGS = -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework GLUT

macos: $(BUILD_DIR) $(OBJ)
	$(CC) $(MACOS_CFLAGS) $(OBJ) -o $(MACOS_EXE) $(MACOS_LDFLAGS)

# --- Common Rules ---
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(if $(findstring linux,$@),$(LINUX_CFLAGS),$(MACOS_CFLAGS)) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean linux macos
