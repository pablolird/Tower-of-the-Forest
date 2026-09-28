# Cross-platform build (macOS / Linux). Windows users can keep using the Visual Studio project.
# SFML 2.6 is required: `brew install sfml@2` on macOS, `apt install libsfml-dev` on Ubuntu.
SFML_DIR ?= $(shell brew --prefix sfml@2 2>/dev/null)
CXX      ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-reorder -Wno-deprecated-declarations
BUILD    := build

ifneq ($(SFML_DIR),)
  CXXFLAGS += -I$(SFML_DIR)/include
  LDFLAGS  += -L$(SFML_DIR)/lib -Wl,-rpath,$(SFML_DIR)/lib
endif

GAME_SRC := $(wildcard *.cpp)
CORE_SRC := Quadtree.cpp EntityManager.cpp Entity.cpp Animation.cpp Vec2.cpp

.PHONY: all run test bench stress clean
all: $(BUILD)/tower-of-the-forest

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/tower-of-the-forest: $(GAME_SRC) *.h | $(BUILD)
	$(CXX) $(CXXFLAGS) $(GAME_SRC) $(LDFLAGS) -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o $@

$(BUILD)/test_quadtree: tests/test_quadtree.cpp bench/entities.h $(CORE_SRC) *.h | $(BUILD)
	$(CXX) $(CXXFLAGS) tests/test_quadtree.cpp $(CORE_SRC) $(LDFLAGS) -lsfml-graphics -lsfml-window -lsfml-system -o $@

$(BUILD)/bench_quadtree: bench/bench_quadtree.cpp bench/*.h $(CORE_SRC) *.h | $(BUILD)
	$(CXX) $(CXXFLAGS) bench/bench_quadtree.cpp $(CORE_SRC) $(LDFLAGS) -lsfml-graphics -lsfml-window -lsfml-system -o $@

run: $(BUILD)/tower-of-the-forest
	./$(BUILD)/tower-of-the-forest

test: $(BUILD)/test_quadtree
	./$(BUILD)/test_quadtree

bench: $(BUILD)/bench_quadtree
	./$(BUILD)/bench_quadtree

# In-game stress test: bench/stress.sh sweeps enemy counts with and without the quadtree
stress: $(BUILD)/tower-of-the-forest
	./bench/stress.sh

clean:
	rm -rf $(BUILD)
