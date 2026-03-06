#ifndef SNAKEMAN_GAME_H
#define SNAKEMAN_GAME_H

#include <stdint.h>

#include "snakeman/scoring.h"
#include "snakeman/state.h"
#include "snakeman/world.h"

typedef enum {
    INPUT_DIR_NONE = 0,
    INPUT_DIR_UP,
    INPUT_DIR_DOWN,
    INPUT_DIR_LEFT,
    INPUT_DIR_RIGHT
} InputDirection;

typedef struct {
    InputDirection direction;
    int start_pressed;
    int pause_pressed;
    int resume_pressed;
    int restart_pressed;
    int quit_pressed;
} InputState;

typedef struct {
    int window_width;
    int window_height;
    int target_fps;
    uint32_t seed;
} GameConfig;

typedef struct {
    GameState state;
    uint32_t tick_count;
    uint32_t run_time_sec;
    uint32_t fixed_steps;
    float second_fraction;
    float game_over_timer;
    ScoreState score;
    HighscoreTable highscores;
    World world;
    uint32_t seed;
    int last_run_rank;       /* rank from highscore insert, -1 if not placed */
} Game;

void game_init(Game *game, const GameConfig *config);
void game_update(Game *game, const InputState *input, float fixed_dt);

#endif
