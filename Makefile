# ==== Configurable Compiler and Base Flags ====

CXX := clang++
CXXFLAGS := -Wall -Wextra -std=c++23

# ==== Mode-Specific Flags ====

RELEASE_FLAGS := -O3 -march=native -flto
DEBUG_FLAGS   := -O0 -g -DDEBUG -fsanitize=address -fno-omit-frame-pointer
SMALL_FLAGS   := -Os -DDEBUG -s -fno-exceptions -fno-rtti -fomit-frame-pointer \
                 -ffunction-sections -fdata-sections
LINK_SMALL    := -Wl,--gc-sections

PROFILE_GEN   := -fprofile-generate
PROFILE_USE   := -fprofile-use=default.profdata

# ==== Sources ====

SRC := $(shell find src -name "*.cpp" -o -name "*.h")
TARGET := build/Render

# Optional auto-detect entry file (comment out next line and uncomment below if needed)
MAIN := src/Render.cpp
# MAIN := $(shell grep -l 'int main' src/*.cpp)

# ==== Targets ====

all: $(TARGET)

$(TARGET): $(MAIN)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -o $@ $^

debug:
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -o $(TARGET) $(MAIN)

small:
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SMALL_FLAGS) $(LINK_SMALL) -o $(TARGET) $(MAIN)

profile-gen:
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(PROFILE_GEN) -o $(TARGET) $(MAIN)

profile-run:
	./$(TARGET)

profile-merge:
	llvm-profdata merge -output=default.profdata default.profraw

profile-use:
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(PROFILE_USE) -flto -o $(TARGET) $(MAIN)

run: $(TARGET)
	clear
	./$(TARGET)

strip:
	strip $(TARGET)

clean:
	rm -rf build default.profraw default.profdata

inspect:
	cat $(MAIN) | less

format:
	@command -v clang-format >/dev/null 2>&1 && \
		clang-format -i $(SRC) || \
		echo "clang-format not found. Install it to enable formatting."

lint:
	@command -v cppcheck >/dev/null 2>&1 && \
		cppcheck --enable=all --std=c++23 --inconclusive --enable=style --quiet src || \
		echo "cppcheck not found. Install it to enable linting."

check: format lint

help:
	@echo ""
	@echo "Build Targets:"
	@echo "  make all            - Build release (optimized) binary"
	@echo "  make debug          - Build with debug symbols and sanitizers"
	@echo "  make small          - Build size-optimized stripped binary"
	@echo ""
	@echo "PGO Targets:"
	@echo "  make profile-gen    - Build with profile instrumentation"
	@echo "  make profile-run    - Run the instrumented binary"
	@echo "  make profile-merge  - Merge profile data to default.profdata"
	@echo "  make profile-use    - Build optimized binary using profile data"
	@echo ""
	@echo "Utility Targets:"
	@echo "  make run            - Clear terminal and run Render"
	@echo "  make strip          - Strip symbols from Render"
	@echo "  make clean          - Delete binaries and profile data"
	@echo "  make inspect        - View main source in pager"
	@echo "  make format         - Format all .cpp/.h files with clang-format"
	@echo "  make lint           - Run static analysis with cppcheck (if installed)"
	@echo "  make check          - Run formatter and linter together"
	@echo "  make help           - Show this help message"
	@echo ""

.PHONY: all debug small run strip clean inspect format lint profile-gen profile-run profile-merge profile-use check help
