#include "snakeman/game.h"

#include <stddef.h>

static TransitionEvent pick_event_from_input(const Game *game, const InputState *input) {
    if (game == NULL || input == NULL) {
        return TRANSITION_EVENT_NONE;
    }

    if (input->quit_pressed) {
        return TRANSITION_EVENT_QUIT;
    }

    switch (game->state) {
        case GAME_STATE_TITLE:
            if (input->start_pressed || input->restart_pressed) {
                return TRANSITION_EVENT_START;
            }
            break;
        case GAME_STATE_RUNNING:
            if (input->pause_pressed) {
                return TRANSITION_EVENT_PAUSE;
            }
            break;
        case GAME_STATE_PAUSED:
            if (input->resume_pressed || input->pause_pressed) {
                return TRANSITION_EVENT_RESUME;
            }
            break;
        case GAME_STATE_GAME_OVER:
            if (input->restart_pressed || input->start_pressed) {
                return TRANSITION_EVENT_SHOW_HIGHSCORES;
            }
            break;
        case GAME_STATE_HIGHSCORES:
            if (input->restart_pressed || input->start_pressed) {
                return TRANSITION_EVENT_RESTART;
            }
            break;
        case GAME_STATE_BOOT:
        case GAME_STATE_QUIT:
        default:
            break;
    }

    return TRANSITION_EVENT_NONE;
}

void game_init(Game *game, const GameConfig *config) {
    (void)config;

    if (game == NULL) {
        return;
    }

    game->state = GAME_STATE_BOOT;
    game->tick_count = 0;
    game->run_time_sec = 0;
    game->fixed_steps = 0;
    game->second_fraction = 0.0f;

    score_init(&game->score);
    highscore_init(&game->highscores);

    game->state = state_next(game->state, TRANSITION_EVENT_NONE);
}

void game_update(Game *game, const InputState *input, float fixed_dt) {
    TransitionEvent event;

    if (game == NULL || input == NULL || fixed_dt <= 0.0f) {
        return;
    }

    event = pick_event_from_input(game, input);
    game->state = state_next(game->state, event);

    if (game->state != GAME_STATE_RUNNING) {
        if (game->state == GAME_STATE_GAME_OVER) {
            game->state = state_next(game->state, TRANSITION_EVENT_SHOW_HIGHSCORES);
        }
        return;
    }

    game->tick_count += 1;
    game->fixed_steps += 1;

    score_update(&game->score, fixed_dt);

    game->second_fraction += fixed_dt;
    if (game->second_fraction >= 1.0f) {
        game->second_fraction -= 1.0f;
        game->run_time_sec += 1;
        score_on_event(&game->score, SCORE_EVENT_SURVIVAL_SECOND);
    }
}
