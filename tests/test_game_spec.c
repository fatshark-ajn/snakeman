#include "test_framework.h"

#include "snakeman/game.h"

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

void test_game_init_sets_title_and_resets_counters(void) {
    Game game;
    GameConfig config = default_config();

    game_init(&game, &config);

    ASSERT_EQ_INT(game.state, GAME_STATE_TITLE);
    ASSERT_EQ_INT(game.tick_count, 0);
    ASSERT_EQ_INT(game.run_time_sec, 0);
    ASSERT_EQ_INT(game.fixed_steps, 0);
    ASSERT_EQ_FLOAT_NEAR(game.second_fraction, 0.0f, 0.0001f);

    ASSERT_EQ_INT(game.score.score, 0);
    ASSERT_EQ_INT(game.score.pickups, 0);
    ASSERT_EQ_FLOAT_NEAR(game.score.combo_multiplier, 1.0f, 0.0001f);
}

void test_game_title_start_enters_running_and_steps_simulation(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    input.start_pressed = 1;

    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
    ASSERT_EQ_INT(game.tick_count, 1);
    ASSERT_EQ_INT(game.fixed_steps, 1);
    ASSERT_EQ_FLOAT_NEAR(game.second_fraction, 0.25f, 0.0001f);
}

void test_game_quit_input_has_priority(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    input.start_pressed = 1;
    input.quit_pressed = 1;

    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_QUIT);
    ASSERT_EQ_INT(game.tick_count, 0);
    ASSERT_EQ_INT(game.fixed_steps, 0);
}

void test_game_running_pause_freezes_simulation(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_PAUSED);
    ASSERT_EQ_INT(game.tick_count, 1);
    ASSERT_EQ_INT(game.fixed_steps, 1);
}

void test_game_paused_resume_reenters_running(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);

    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    input.pause_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    input.resume_pressed = 1;
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.state, GAME_STATE_RUNNING);
    ASSERT_EQ_INT(game.tick_count, 2);
    ASSERT_EQ_INT(game.fixed_steps, 2);
}

void test_game_survival_points_increment_once_per_second(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);

    input.start_pressed = 1;
    game_update(&game, &input, 0.25f);

    input = no_input();
    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);
    game_update(&game, &input, 0.25f);

    ASSERT_EQ_INT(game.run_time_sec, 1);
    ASSERT_EQ_INT(game.score.score, 1);
    ASSERT_EQ_INT(game.score.survival_seconds_accum, 1);
}

void test_game_survival_awards_multiple_seconds_for_large_dt_spec(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);

    input.start_pressed = 1;
    game_update(&game, &input, 0.1f);

    input = no_input();
    game_update(&game, &input, 2.4f);

    ASSERT_EQ_INT(game.run_time_sec, 2);
    ASSERT_EQ_INT(game.score.survival_seconds_accum, 2);
}

void test_game_pause_freezes_score_timers(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();
    float combo_timer_before;

    game_init(&game, &config);

    input.start_pressed = 1;
    game_update(&game, &input, 0.1f);

    score_on_event(&game.score, SCORE_EVENT_PICKUP);
    combo_timer_before = game.score.combo_timer_sec;

    input = no_input();
    input.pause_pressed = 1;
    game_update(&game, &input, 0.5f);

    input = no_input();
    game_update(&game, &input, 2.0f);

    ASSERT_EQ_INT(game.state, GAME_STATE_PAUSED);
    ASSERT_EQ_FLOAT_NEAR(game.score.combo_timer_sec, combo_timer_before, 0.0001f);
}

void test_game_over_screen_has_minimum_lock_before_highscores_spec(void) {
    Game game;
    GameConfig config = default_config();
    InputState input = no_input();

    game_init(&game, &config);
    game.state = GAME_STATE_GAME_OVER;

    game_update(&game, &input, 0.1f);

    ASSERT_EQ_INT(game.state, GAME_STATE_GAME_OVER);
}
