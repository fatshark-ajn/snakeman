#include "test_framework.h"

#include "snakeman/game.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static GameConfig default_config(void) {
    GameConfig config;
    config.window_width = 1280;
    config.window_height = 720;
    config.target_fps = 60;
    config.seed = 1234;
    return config;
}

static InputState no_input(void) {
    InputState input;
    input.direction = INPUT_DIR_NONE;
    input.start_pressed = 0;
    input.pause_pressed = 0;
    input.resume_pressed = 0;
    input.restart_pressed = 0;
    input.quit_pressed = 0;
    return input;
}

/* Helper to advance game into RUNNING state */
static void enter_running(Game *game) {
    InputState input = no_input();
    GameConfig config = default_config();

    game_init(game, &config);
    input.start_pressed = 1;
    game_update(game, &input, 0.25f);
}

/* ------------------------------------------------------------------ */
/* game_init: comprehensive checks                                     */
/* ------------------------------------------------------------------ */

void test_game_init_null_game_is_safe(void) {
    GameConfig config = default_config();
    game_init(NULL, &config);
    ASSERT_TRUE(1);
}

void test_game_init_null_config_is_safe(void) {
    Game game;
    game_init(&game, NULL);
    /* Should still reach TITLE state since config is (void)config'd */
    ASSERT_EQ_INT(game.state, GAME_STATE_TITLE);
}

void test_game_init_state_is_title(void) {
    Game game;
    GameConfig config = default_config();
    game_init(&game, &config);
    ASSERT_EQ_INT(game.state, GAME_STATE_TITLE);
}

void test_game_init_tick_count_zero(void) {
    Game game;
    GameConfig config = default_config();
    game_init(&game, &config);
    ASSERT_EQ_INT(game.tick_count, 0);
}

void test_game_init_run_time_zero(void) {
    Game game;
    GameConfig config = default_config();
    game_init(&game, &config);
    ASSERT_EQ_INT(game.run_time_sec, 0);
}

void test_game_init_fixed_steps_zero(void) {
    Game game;
    GameConfig config = default_config();
    game_init(&game, &config);
    ASSERT_EQ_INT(game.fixed_steps, 0);
}

void test_game_init_second_fraction_zero(void) {
    Game game;
    GameConfig config = default_config();
    game_init(&game, &config);
    ASSERT_EQ_FLOAT_NEAR(game.second_fraction, 0.0f, 0.0001f);
}

void test_game_init_score_state_zeroed(void) {
    Game game;
    GameConfig config = default_config();
    game_init(&game, &config);
    ASSERT_EQ_INT(game.score.score, 0);
    ASSERT_EQ_INT(game.score.pickups, 0);
    ASSERT_EQ_FLOAT_NEAR(game.score.combo_multiplier, 1.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(game.score.combo_timer_sec, 0.0f, 0.0001f);
}

void test_game_init_highscore_table_zeroed(void) {
    Game game;
    GameConfig config = default_config();
    int i;
    game_init(&game, &config);

    for (i = 0; i < 10; ++i) {
        ASSERT_EQ_INT(game.highscores.entries[i].score, 0);
    }
}

void test_game_init_can_be_called_multiple_times(void) {
    Game game;
    GameConfig config = default_config();

    game_init(&game, &config);
    enter_running(&game);
    ASSERT_TRUE(game.tick_count > 0);

    game_init(&game, &config);
    ASSERT_EQ_INT(game.state, GAME_STATE_TITLE);
    ASSERT_EQ_INT(game.tick_count, 0);
}

/* ------------------------------------------------------------------ */
/* game_update: null safety                                            */
/* ------------------------------------------------------------------ */

void test_game_update_null_game_is_safe(void) {
    InputState input = no_input();
    game_update(NULL, &input, 0.25f);
    ASSERT_TRUE(1);
}

void test_game_update_null_input_is_safe(void) {
    Game game;
    GameConfig config = default_config();
    game_init(&game, &config);
    game_update(&game, NULL, 0.25f);
    ASSERT_TRUE(1);
}

void test_game_update_zero_dt_is_noop(void) {
    Game game;
    InputState input = no_input();
    uint32_t ticks;

    enter_running(&game);
    ticks = game.tick_count;

    game_update(&game, &input, 0.0f);
    ASSERT_EQ_INT(game.tick_count, (int)ticks);
}

void test_game_update_negative_dt_is_noop(void) {
    Game game;
    InputState input = no_input();
    uint32_t ticks;

    enter_running(&game);
    ticks = game.tick_count;

    game_update(&game, &input, -1.0f);
    ASSERT_EQ_INT(game.tick_count, (int)ticks);
}

/* ------------------------------------------------------------------ */
/* State transitions via game_update input                             */
/* ------------------------------------------------------------------ */

void test_game_title_to_running_via_start(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
}

void test_game_title_to_running_via_restart(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    input.restart_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
}

void test_game_title_to_quit(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

void test_game_title_no_input_stays_title(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_TITLE);
}

void test_game_running_to_paused(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_PAUSED);
}

void test_game_paused_to_running_via_resume(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    input.resume_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
}

void test_game_paused_to_running_via_pause_toggle(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_PAUSED);

    /* Pressing pause again from paused triggers RESUME */
    input = no_input();
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
}

void test_game_running_quit_from_running(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

void test_game_paused_quit(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

/* ------------------------------------------------------------------ */
/* Quit priority: quit overrides all other inputs                      */
/* ------------------------------------------------------------------ */

void test_game_quit_priority_over_start(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    input.start_pressed = 1;
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

void test_game_quit_priority_over_pause(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    input.pause_pressed = 1;
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

void test_game_quit_priority_over_resume(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    input.resume_pressed = 1;
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

void test_game_quit_priority_over_restart(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    /* Force into HIGHSCORES for restart path */
    game.state = GAME_STATE_HIGHSCORES;

    input.restart_pressed = 1;
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

/* ------------------------------------------------------------------ */
/* Simulation: ticks only advance in RUNNING                           */
/* ------------------------------------------------------------------ */

void test_game_ticks_advance_in_running(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    ASSERT_EQ_INT(game.tick_count, 1);

    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.tick_count, 2);

    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.tick_count, 3);
}

void test_game_ticks_frozen_in_title(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    ASSERT_EQ_INT(game.tick_count, 0);

    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.tick_count, 0);

    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.tick_count, 0);
}

void test_game_ticks_frozen_in_paused(void) {
    Game game;
    InputState input = no_input();
    uint32_t ticks_before;

    enter_running(&game);
    ticks_before = game.tick_count;

    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.tick_count, (int)ticks_before);
}

void test_game_fixed_steps_track_ticks(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    ASSERT_EQ_INT(game.fixed_steps, 1);

    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.fixed_steps, 3);
}

/* ------------------------------------------------------------------ */
/* Simulation: second accumulation and survival points                 */
/* ------------------------------------------------------------------ */

void test_game_second_fraction_accumulates(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    ASSERT_EQ_FLOAT_NEAR(game.second_fraction, 0.25f, 0.0001f);

    game_update(&game, &input, 0.25f);
    ASSERT_EQ_FLOAT_NEAR(game.second_fraction, 0.5f, 0.0001f);

    game_update(&game, &input, 0.25f);
    ASSERT_EQ_FLOAT_NEAR(game.second_fraction, 0.75f, 0.0001f);
}

void test_game_second_fraction_wraps_at_one(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);

    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);

    /* 0.25 * 4 = 1.0 -> wraps to 0.0 */
    ASSERT_EQ_FLOAT_NEAR(game.second_fraction, 0.0f, 0.0001f);
    ASSERT_EQ_INT(game.run_time_sec, 1);
}

void test_game_survival_point_awarded_at_one_second(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);

    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.score.score, 1);
    ASSERT_EQ_INT(game.score.survival_seconds_accum, 1);
}

void test_game_no_survival_point_before_one_second(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);

    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);

    /* 0.75 seconds total -- no survival point yet */
    ASSERT_EQ_INT(game.score.score, 0);
    ASSERT_EQ_INT(game.score.survival_seconds_accum, 0);
}

void test_game_survival_accumulates_over_time(void) {
    Game game;
    InputState input = no_input();
    int i;

    enter_running(&game);

    /* Run for 5 seconds (20 updates of 0.25 each, minus the initial one) */
    for (i = 0; i < 19; ++i) {
        game_update(&game, &input, 0.25f);
    }

    /* Total updates = 20, total time = 5.0s -> 5 survival points */
    ASSERT_EQ_INT(game.run_time_sec, 5);
    ASSERT_EQ_INT(game.score.survival_seconds_accum, 5);
    ASSERT_EQ_INT(game.score.score, 5);
}

/* ------------------------------------------------------------------ */
/* Paused state: timers frozen                                         */
/* ------------------------------------------------------------------ */

void test_game_pause_freezes_tick_count(void) {
    Game game;
    InputState input = no_input();
    uint32_t ticks;

    enter_running(&game);
    ticks = game.tick_count;

    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    game_update(&game, &input, 5.0f);

    ASSERT_EQ_INT(game.tick_count, (int)ticks);
}

void test_game_pause_freezes_second_fraction(void) {
    Game game;
    InputState input = no_input();
    float frac;

    enter_running(&game);
    frac = game.second_fraction;

    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    game_update(&game, &input, 5.0f);

    ASSERT_EQ_FLOAT_NEAR(game.second_fraction, frac, 0.0001f);
}

void test_game_pause_freezes_run_time(void) {
    Game game;
    InputState input = no_input();
    uint32_t time_before;

    enter_running(&game);
    time_before = game.run_time_sec;

    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    game_update(&game, &input, 10.0f);

    ASSERT_EQ_INT(game.run_time_sec, (int)time_before);
}

void test_game_pause_freezes_combo_timer(void) {
    Game game;
    InputState input = no_input();
    float combo_before;

    enter_running(&game);

    /* Add a pickup to start combo timer */
    score_on_event(&game.score, SCORE_EVENT_PICKUP);
    combo_before = game.score.combo_timer_sec;

    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    game_update(&game, &input, 10.0f);

    ASSERT_EQ_FLOAT_NEAR(game.score.combo_timer_sec, combo_before, 0.0001f);
}

void test_game_resume_continues_from_paused_state(void) {
    Game game;
    InputState input = no_input();
    uint32_t ticks;

    enter_running(&game);
    ticks = game.tick_count;

    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    game_update(&game, &input, 1.0f);
    ASSERT_EQ_INT(game.tick_count, (int)ticks);

    input.resume_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.tick_count, (int)ticks + 1);
}

/* ------------------------------------------------------------------ */
/* Score interaction via game_update                                   */
/* ------------------------------------------------------------------ */

void test_game_combo_timer_ticks_down_in_running(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);

    score_on_event(&game.score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(game.score.combo_timer_sec, 3.5f, 0.0001f);

    game_update(&game, &input, 1.0f);
    ASSERT_EQ_FLOAT_NEAR(game.score.combo_timer_sec, 2.5f, 0.0001f);
}

void test_game_combo_expires_during_running(void) {
    Game game;
    InputState input = no_input();
    int i;

    enter_running(&game);

    score_on_event(&game.score, SCORE_EVENT_PICKUP);
    score_on_event(&game.score, SCORE_EVENT_PICKUP);
    ASSERT_TRUE(game.score.combo_multiplier > 1.0f);

    /* Advance 4 seconds to expire combo */
    for (i = 0; i < 16; ++i) {
        game_update(&game, &input, 0.25f);
    }

    ASSERT_EQ_FLOAT_NEAR(game.score.combo_multiplier, 1.0f, 0.0001f);
}

/* ------------------------------------------------------------------ */
/* GAME_OVER -> HIGHSCORES auto-transition in current implementation   */
/* ------------------------------------------------------------------ */

void test_game_game_over_auto_transitions_to_highscores(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    game.state = GAME_STATE_GAME_OVER;

    game_update(&game, &input, 0.25f);

    /* Current implementation immediately transitions */
    ASSERT_EQ_INT(game.state, GAME_STATE_HIGHSCORES);
}

/* ------------------------------------------------------------------ */
/* HIGHSCORES -> RUNNING restart                                       */
/* ------------------------------------------------------------------ */

void test_game_highscores_restart_enters_running(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    game.state = GAME_STATE_HIGHSCORES;

    input.restart_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
}

void test_game_highscores_start_enters_running(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    game.state = GAME_STATE_HIGHSCORES;

    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
}

void test_game_highscores_quit(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    game.state = GAME_STATE_HIGHSCORES;

    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

void test_game_highscores_no_input_stays(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    game.state = GAME_STATE_HIGHSCORES;

    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_HIGHSCORES);
}

/* ------------------------------------------------------------------ */
/* GAME_OVER input handling                                            */
/* ------------------------------------------------------------------ */

void test_game_game_over_start_shows_highscores(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    game.state = GAME_STATE_GAME_OVER;

    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_HIGHSCORES);
}

void test_game_game_over_restart_shows_highscores(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    game.state = GAME_STATE_GAME_OVER;

    input.restart_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_HIGHSCORES);
}

void test_game_game_over_quit(void) {
    Game game;
    InputState input = no_input();

    enter_running(&game);
    game.state = GAME_STATE_GAME_OVER;

    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

/* ------------------------------------------------------------------ */
/* Full gameplay lifecycle                                             */
/* ------------------------------------------------------------------ */

void test_game_full_lifecycle_title_run_die_highscores_restart(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    ASSERT_EQ_INT(game.state, GAME_STATE_TITLE);

    /* Start */
    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);

    /* Play a bit */
    input = no_input();
    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);

    /* Simulate death */
    game.state = GAME_STATE_GAME_OVER;
    game_update(&game, &input, 0.25f);
    /* Auto-transitions to highscores */
    ASSERT_EQ_INT(game.state, GAME_STATE_HIGHSCORES);

    /* Restart */
    input.restart_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
}

void test_game_full_lifecycle_title_run_pause_quit(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);

    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);

    input = no_input();
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_PAUSED);

    input = no_input();
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

/* ------------------------------------------------------------------ */
/* QUIT is terminal via game_update                                    */
/* ------------------------------------------------------------------ */

void test_game_quit_is_terminal(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);

    /* Additional updates with various inputs should stay in QUIT */
    input = no_input();
    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);

    input = no_input();
    input.restart_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
}

void test_game_no_simulation_in_quit(void) {
    Game game;
    InputState input = no_input();
    uint32_t ticks;

    enter_running(&game);
    ticks = game.tick_count;

    input.quit_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    game_update(&game, &input, 1.0f);
    game_update(&game, &input, 1.0f);

    ASSERT_EQ_INT(game.tick_count, (int)ticks);
}

/* ------------------------------------------------------------------ */
/* InputDirection enum: verify values exist                            */
/* ------------------------------------------------------------------ */

void test_input_direction_enum_values(void) {
    ASSERT_EQ_INT(INPUT_DIR_NONE, 0);
    ASSERT_TRUE(INPUT_DIR_UP != INPUT_DIR_DOWN);
    ASSERT_TRUE(INPUT_DIR_LEFT != INPUT_DIR_RIGHT);
    ASSERT_TRUE(INPUT_DIR_UP != INPUT_DIR_LEFT);
}

/* ------------------------------------------------------------------ */
/* Edge cases: rapid state changes                                     */
/* ------------------------------------------------------------------ */

void test_game_rapid_start_pause_resume(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();
    int i;

    game_init(&game, &config);

    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);
    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);

    for (i = 0; i < 50; ++i) {
        input = no_input();
        input.pause_pressed = 1;
        game_update(&game, &input, 0.016f);
        ASSERT_EQ_INT(game.state, GAME_STATE_PAUSED);

        input = no_input();
        input.resume_pressed = 1;
        game_update(&game, &input, 0.016f);
        ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
    }
}

void test_game_multiple_no_input_updates(void) {
    Game game;
    InputState input = no_input();
    int i;

    enter_running(&game);

    for (i = 0; i < 1000; ++i) {
        game_update(&game, &input, 0.016f);
    }

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
    ASSERT_TRUE(game.tick_count > 1000);
}
