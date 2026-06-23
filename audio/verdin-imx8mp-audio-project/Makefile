# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2 -Iinclude -DDEBUG -DENABLE_TESTS
LDFLAGS = -lasound -lm -lpthread

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests

# Target executable
TARGET = $(BIN_DIR)/audio_player

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/audio_engine.c \
          $(SRC_DIR)/wav_loader.c \
          $(SRC_DIR)/utils.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Test executable
TEST_TARGET = $(BIN_DIR)/test_audio

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build test
test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_DIR)/test_audio.c $(OBJECTS)
	$(CC) $(CFLAGS) $< $(OBJECTS) -o $@ $(LDFLAGS)

# Clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Install to target
install:
	@echo "Copying to target board..."
	scp $(TARGET) torizon@192.168.1.100:/home/torizon/

# Run on target
run:
	ssh torizon@192.168.1.100 "cd /home/torizon && ./audio_player -s 440"

# Deploy and run
deploy: install run

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the application"
	@echo "  clean    - Remove built files"
	@echo "  test     - Build test executable"
	@echo "  install  - Copy to target board (adjust IP)"
	@echo "  run      - Run on target board"
	@echo "  deploy   - Install and run"

.PHONY: all clean test install run deploy help directories
