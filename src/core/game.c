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

static Direction input_to_direction(InputDirection d) {
    switch (d) {
        case INPUT_DIR_UP:    return DIR_UP;
        case INPUT_DIR_DOWN:  return DIR_DOWN;
        case INPUT_DIR_LEFT:  return DIR_LEFT;
        case INPUT_DIR_RIGHT: return DIR_RIGHT;
        default:              return (Direction)(-1);
    }
}

static void start_new_run(Game *game) {
    game->tick_count = 0;
    game->run_time_sec = 0;
    game->fixed_steps = 0;
    game->second_fraction = 0.0f;
    game->game_over_timer = 0.0f;
    game->last_run_rank = -1;
    score_init(&game->score);

    /* Generate new seed from previous state */
    game->seed = game->seed * 6364136223846793005ULL + 1442695040888963407ULL;
    world_init(&game->world, game->seed);
}

void game_init(Game *game, const GameConfig *config) {
    if (game == NULL) {
        return;
    }

    game->state = GAME_STATE_BOOT;
    game->tick_count = 0;
    game->run_time_sec = 0;
    game->fixed_steps = 0;
    game->second_fraction = 0.0f;
    game->game_over_timer = 0.0f;
    game->seed = (config != NULL) ? config->seed : 1337;
    game->last_run_rank = -1;

    score_init(&game->score);
    highscore_init(&game->highscores);

    game->state = state_next(game->state, TRANSITION_EVENT_NONE);
}

void game_update(Game *game, const InputState *input, float fixed_dt) {
    TransitionEvent event;
    GameState prev_state;

    if (game == NULL || input == NULL || fixed_dt <= 0.0f) {
        return;
    }

    prev_state = game->state;
    event = pick_event_from_input(game, input);
    game->state = state_next(game->state, event);

    /* Initialize world on transition to RUNNING */
    if (game->state == GAME_STATE_RUNNING && prev_state != GAME_STATE_RUNNING
        && prev_state != GAME_STATE_PAUSED) {
        start_new_run(game);
    }

    /* Reset game-over timer on entry */
    if (game->state == GAME_STATE_GAME_OVER && prev_state != GAME_STATE_GAME_OVER) {
        game->game_over_timer = 0.0f;
    }

    if (game->state == GAME_STATE_GAME_OVER) {
        game->game_over_timer += fixed_dt;
        if (game->game_over_timer >= 0.2f) {
            game->state = state_next(game->state, TRANSITION_EVENT_SHOW_HIGHSCORES);
            /* Finalize score on transition to HIGHSCORES */
            if (game->state == GAME_STATE_HIGHSCORES) {
                ScoreEntry entry = score_finalize_run(&game->score, game->run_time_sec, game->seed);
                game->last_run_rank = highscore_try_insert(&game->highscores, entry);
            }
        }
        return;
    }

    /* Also finalize if we reach HIGHSCORES via RUN_COMPLETE (all pickups cleared) */
    if (game->state == GAME_STATE_HIGHSCORES && prev_state != GAME_STATE_HIGHSCORES) {
        ScoreEntry entry = score_finalize_run(&game->score, game->run_time_sec, game->seed);
        game->last_run_rank = highscore_try_insert(&game->highscores, entry);
    }

    if (game->state != GAME_STATE_RUNNING) {
        return;
    }

    game->tick_count += 1;
    game->fixed_steps += 1;

    /* Apply powerup effects on scoring state each tick */
    {
        PowerupType active = game->world.active_powerup.type;

        /* Overclock: combo window = 2.5s while active */
        if (active == POWERUP_OVERCLOCK) {
            game->score.combo_default_window_sec = 2.5f;
        } else {
            /* Base 3.5s minus difficulty shrink (0.2s per milestone, floor 2.2s) */
            float base_window = 3.5f - game->world.combo_window_shrink;
            if (base_window < 2.2f) base_window = 2.2f;
            game->score.combo_default_window_sec = base_window;
        }

        /* Multiplier Orb: extend combo cap by +0.5 */
        if (active == POWERUP_MULTIPLIER_ORB) {
            game->score.combo_cap = 4.5f;
        } else {
            game->score.combo_cap = 4.0f;
        }
    }

    /* Run world simulation */
    {
        Direction dir = input_to_direction(input->direction);
        int ev = world_update(&game->world, dir, fixed_dt);

        if (ev & WORLD_EVENT_PLAYER_DIED) {
            game->state = state_next(game->state, TRANSITION_EVENT_PLAYER_DIED);
            return;
        }

        if (ev & WORLD_EVENT_PICKUP) {
            /* Multiplier Orb: save/restore multiplier to freeze it */
            float saved_mult = game->score.combo_multiplier;
            int orb_active = (game->world.active_powerup.type == POWERUP_MULTIPLIER_ORB);

            score_on_event(&game->score, SCORE_EVENT_PICKUP);

            if (orb_active) {
                /* Freeze: restore multiplier to pre-pickup value
                   (the pickup still scores at the frozen multiplier because
                    pickup_points() reads the multiplier before increment) */
                game->score.combo_multiplier = saved_mult;
                /* But keep max_combo_multiplier updated */
                if (saved_mult > game->score.max_combo_multiplier) {
                    game->score.max_combo_multiplier = saved_mult;
                }
            }

            if (ev & WORLD_EVENT_RISK_PICKUP) {
                score_on_event(&game->score, SCORE_EVENT_RISK_PICKUP);
            }
            if (ev & WORLD_EVENT_MAGNET_PICKUP) {
                score_on_event(&game->score, SCORE_EVENT_MAGNET_PICKUP);
            }
            if (ev & WORLD_EVENT_EMP_PICKUP) {
                score_on_event(&game->score, SCORE_EVENT_EMP_PICKUP);
            }
        }

        if (ev & WORLD_EVENT_ALL_CLEARED) {
            /* Unused shield bonus: +100 */
            if (game->world.player.has_shield) {
                score_on_event(&game->score, SCORE_EVENT_UNUSED_SHIELD);
            }
            game->state = state_next(game->state, TRANSITION_EVENT_RUN_COMPLETE);
            return;
        }
    }

    /* Multiplier Orb: freeze combo timer (don't let it decay) */
    if (game->world.active_powerup.type == POWERUP_MULTIPLIER_ORB) {
        /* Skip normal combo decay — keep timer and multiplier frozen */
    } else {
        score_update(&game->score, fixed_dt);
    }

    game->second_fraction += fixed_dt;
    while (game->second_fraction >= 1.0f) {
        game->second_fraction -= 1.0f;
        game->run_time_sec += 1;
        score_on_event(&game->score, SCORE_EVENT_SURVIVAL_SECOND);
    }
}
