CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS ?=

SDL_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null || pkg-config --cflags sdl2 2>/dev/null)
SDL_LIBS = $(shell sdl2-config --libs 2>/dev/null || pkg-config --libs sdl2 2>/dev/null) -lSDL2_image

BUILD_DIR := build
GAME_BIN := $(BUILD_DIR)/snakeman
TEST_BIN := $(BUILD_DIR)/snakeman_tests

CORE_SOURCES := src/core/game.c src/core/scoring.c src/core/state.c src/core/world.c
GAME_SOURCES := src/main.c src/platform/sdl_app.c $(CORE_SOURCES)
TEST_SOURCES := tests/test_framework.c tests/test_main.c tests/test_state.c tests/test_state_spec.c tests/test_state_extended.c tests/test_scoring.c tests/test_scoring_spec.c tests/test_scoring_extended.c tests/test_highscore_extended.c tests/test_game_spec.c tests/test_game_extended.c $(CORE_SOURCES)

.PHONY: all run test clean check-sdl

all: $(GAME_BIN)

$(GAME_BIN): $(GAME_SOURCES) | $(BUILD_DIR) check-sdl
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $^ -o $@ $(LDFLAGS) $(SDL_LIBS)

$(TEST_BIN): $(TEST_SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

check-sdl:
	@if [ -z "$(SDL_CFLAGS)" ] || [ -z "$(SDL_LIBS)" ]; then \
		printf "SDL2 development files not found. Install SDL2 and ensure sdl2-config or pkg-config is available.\n"; \
		exit 1; \
	fi

run: $(GAME_BIN)
	./$(GAME_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)
