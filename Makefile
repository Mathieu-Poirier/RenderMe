# ==== Configurable Compiler and Base Flags ====

CXX := clang++
CXXFLAGS := -Wall -Wextra -std=c++23 -Iengine

# ==== Mode-Specific Flags ====

RELEASE_FLAGS := -O3 -march=native -flto
DEBUG_FLAGS   := -O0 -g -DDEBUG -fsanitize=address -fno-omit-frame-pointer
SMALL_FLAGS   := -Os -DDEBUG -s -fno-exceptions -fno-rtti -fomit-frame-pointer \
                 -ffunction-sections -fdata-sections
LINK_SMALL    := -Wl,--gc-sections

PROFILE_GEN   := -fprofile-generate
PROFILE_USE   := -fprofile-use=default.profdata

# ==== Directories ====

ENGINE_DIR := engine
demo_DIR   := demos
BUILD_DIR  := build

# ==== Sources ====

ENGINE_SRC := $(shell find $(ENGINE_DIR) -name "*.cpp" -o -name "*.h")
DEMO_SRC   := $(wildcard $(demo_DIR)/*.cpp)
DEMO_BIN   := $(patsubst $(demo_DIR)/%.cpp,$(BUILD_DIR)/%,$(DEMO_SRC))

# ==== Targets ====

.PHONY: all demos build-demo run-demo debug small profile-gen profile-use strip clean lint help run-demo

# Build all demos
all: demos

demos: $(DEMO_BIN)

# Build a single demo
build-demo:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -o $(BUILD_DIR)/$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

# Run a single demo
run-demo:
	@./$(BUILD_DIR)/$(NAME)

# Builds a single demo in debug mode
debug:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -o $(BUILD_DIR)/debug_$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

# Build a size-optimized demo
small:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SMALL_FLAGS) $(LINK_SMALL) -o $(BUILD_DIR)/small_$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

# PGO targets
profile-gen:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(PROFILE_GEN) -o $(BUILD_DIR)/profile_$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

profile-use:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(PROFILE_USE) -flto -o $(BUILD_DIR)/profile_use_$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

# Strip symbols from output
strip:
	strip $(BUILD_DIR)/$(NAME)

# Remove all build artifacts
clean:
	rm -rf $(BUILD_DIR) default.profraw default.profdata

# Static analysis
lint:
	@command -v cppcheck >/dev/null 2>&1 && \
		cppcheck --enable=all --std=c++23 --inconclusive --enable=style --quiet $(ENGINE_SRC) $(DEMO_SRC) || \
		echo "cppcheck not found. Install it to enable linting."

# Help message
help:
	@echo ""
	@echo "Build Targets:"
	@echo "  make                - Build all demos"
	@echo "  make demos          - Alias for 'make'"
	@echo "  make build-demo NAME=<DemoName> - Compile a specific demo"
	@echo "  make run-demo NAME=<DemoName>   - Build + run a specific demo"
	@echo ""
	@echo "Configuration Targets:"
	@echo "  make debug          - Debug build of a specific demo"
	@echo "  make small          - Smallest possible binary build"
	@echo ""
	@echo "PGO Targets:"
	@echo "  make profile-gen    - Profile generation build"
	@echo "  make profile-use    - Use collected profile data"
	@echo ""
	@echo "Utility Targets:"
	@echo "  make strip          - Strip symbols from demo binary"
	@echo "  make clean          - Remove all build artifacts"
	@echo "  make lint           - Run static analysis (cppcheck)"
	@echo "  make help           - Show this help message"
	@echo ""
