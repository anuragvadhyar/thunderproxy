# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/opt/homebrew/include -Iinclude
LDFLAGS = -lcurl

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
CONFIG_DIR = config

# Files
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/loadbalancer.cpp $(SRC_DIR)/http.cpp $(SRC_DIR)/thread_pool.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET = loadbalancer

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Run
run: $(TARGET)
	./$(TARGET)

# Install dependencies (macOS)
install-deps:
	brew install curl nlohmann-json

.PHONY: all clean run install-deps
