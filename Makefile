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

.PHONY: all demos run-demo debug small profile-gen profile-use strip clean format lint help
all: demos

demos: $(DEMO_BIN)

$(BUILD_DIR)/%: $(demo_DIR)/%.cpp $(ENGINE_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -o $@ $< $(ENGINE_SRC)

# Run a specific demo by name: make run-demo NAME=DemoName
run-demo: demos
	@./$(BUILD_DIR)/$(NAME)

# Builds for a single demo in different modes
debug: $(DEMO_BIN)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -o $(BUILD_DIR)/debug_$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

small: $(DEMO_BIN)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SMALL_FLAGS) $(LINK_SMALL) -o $(BUILD_DIR)/small_$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

profile-gen:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(PROFILE_GEN) -o $(BUILD_DIR)/profile_$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

profile-use:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(PROFILE_USE) -flto -o $(BUILD_DIR)/profile_use_$(NAME) $(demo_DIR)/$(NAME).cpp $(ENGINE_SRC)

# Strip symbols
strip:
	strip $(BUILD_DIR)/$(NAME)

# Remove build artifacts
clean:
	rm -rf $(BUILD_DIR) default.profraw default.profdata

# Format source files
format:
	@command -v clang-format >/dev/null 2>&1 && \
		clang-format -i $(ENGINE_SRC) $(DEMO_SRC) || \
		echo "clang-format not found. Install it to enable formatting."

# Static analysis
lint:
	@command -v cppcheck >/dev/null 2>&1 && \
		cppcheck --enable=all --std=c++23 --inconclusive --enable=style --quiet $(ENGINE_SRC) $(DEMO_SRC) || \
		echo "cppcheck not found. Install it to enable linting."

# Help message
help:
	@echo ""
	@echo "Build Targets:"
	@echo "  make            - Build all demos"
	@echo "  make demos      - Alias for 'make'"
	@echo "  make run-demo NAME=<DemoName> - Run a specific demo do not use quotation marks"
	@echo ""
	@echo "Configuration Targets:"
	@echo "  make debug      - Build debug version of a demo"
	@echo "  make small      - Build size-optimized demo"
	@echo ""
	@echo "PGO Targets:"
	@echo "  make profile-gen   - Build with instrumentation"
	@echo "  make profile-use   - Build optimized with profile data"
	@echo ""
	@echo "Utility Targets:"
	@echo "  make strip       - Strip symbols"
	@echo "  make clean       - Remove build artifacts"
	@echo "  make format      - Run clang-format"
	@echo "  make lint        - Run cppcheck"
	@echo "  make help        - Show this help message"
