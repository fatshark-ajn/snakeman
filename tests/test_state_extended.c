#include "test_framework.h"

#include "snakeman/state.h"

/* ------------------------------------------------------------------ */
/* Exhaustive per-state coverage: every (state, event) pair tested    */
/* ------------------------------------------------------------------ */

void test_state_boot_none_goes_title(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_NONE), GAME_STATE_TITLE);
}

void test_state_boot_start_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_START), GAME_STATE_BOOT);
}

void test_state_boot_pause_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_PAUSE), GAME_STATE_BOOT);
}

void test_state_boot_resume_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_RESUME), GAME_STATE_BOOT);
}

void test_state_boot_back_to_title_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_BACK_TO_TITLE), GAME_STATE_BOOT);
}

void test_state_boot_player_died_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_PLAYER_DIED), GAME_STATE_BOOT);
}

void test_state_boot_run_complete_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_RUN_COMPLETE), GAME_STATE_BOOT);
}

void test_state_boot_show_highscores_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_SHOW_HIGHSCORES), GAME_STATE_BOOT);
}

void test_state_boot_restart_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_RESTART), GAME_STATE_BOOT);
}

void test_state_boot_quit_goes_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_BOOT, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
}

/* TITLE exhaustive */

void test_state_title_none_stays_title(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_NONE), GAME_STATE_TITLE);
}

void test_state_title_start_goes_running(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_START), GAME_STATE_RUNNING);
}

void test_state_title_pause_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_PAUSE), GAME_STATE_TITLE);
}

void test_state_title_resume_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_RESUME), GAME_STATE_TITLE);
}

void test_state_title_back_to_title_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_BACK_TO_TITLE), GAME_STATE_TITLE);
}

void test_state_title_player_died_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_PLAYER_DIED), GAME_STATE_TITLE);
}

void test_state_title_run_complete_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_RUN_COMPLETE), GAME_STATE_TITLE);
}

void test_state_title_show_highscores_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_SHOW_HIGHSCORES), GAME_STATE_TITLE);
}

void test_state_title_restart_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_RESTART), GAME_STATE_TITLE);
}

void test_state_title_quit_goes_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
}

/* RUNNING exhaustive */

void test_state_running_none_stays_running(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_NONE), GAME_STATE_RUNNING);
}

void test_state_running_start_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_START), GAME_STATE_RUNNING);
}

void test_state_running_pause_goes_paused(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_PAUSE), GAME_STATE_PAUSED);
}

void test_state_running_resume_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_RESUME), GAME_STATE_RUNNING);
}

void test_state_running_back_to_title_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_BACK_TO_TITLE), GAME_STATE_RUNNING);
}

void test_state_running_player_died_goes_game_over(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_PLAYER_DIED), GAME_STATE_GAME_OVER);
}

void test_state_running_run_complete_goes_highscores(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_RUN_COMPLETE), GAME_STATE_HIGHSCORES);
}

void test_state_running_show_highscores_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_SHOW_HIGHSCORES), GAME_STATE_RUNNING);
}

void test_state_running_restart_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_RESTART), GAME_STATE_RUNNING);
}

void test_state_running_quit_goes_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
}

/* PAUSED exhaustive */

void test_state_paused_none_stays_paused(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_NONE), GAME_STATE_PAUSED);
}

void test_state_paused_start_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_START), GAME_STATE_PAUSED);
}

void test_state_paused_pause_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_PAUSE), GAME_STATE_PAUSED);
}

void test_state_paused_resume_goes_running(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_RESUME), GAME_STATE_RUNNING);
}

void test_state_paused_back_to_title_goes_title(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_BACK_TO_TITLE), GAME_STATE_TITLE);
}

void test_state_paused_player_died_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_PLAYER_DIED), GAME_STATE_PAUSED);
}

void test_state_paused_run_complete_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_RUN_COMPLETE), GAME_STATE_PAUSED);
}

void test_state_paused_show_highscores_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_SHOW_HIGHSCORES), GAME_STATE_PAUSED);
}

void test_state_paused_restart_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_RESTART), GAME_STATE_PAUSED);
}

void test_state_paused_quit_goes_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
}

/* GAME_OVER exhaustive */

void test_state_game_over_none_stays_game_over(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_NONE), GAME_STATE_GAME_OVER);
}

void test_state_game_over_start_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_START), GAME_STATE_GAME_OVER);
}

void test_state_game_over_pause_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_PAUSE), GAME_STATE_GAME_OVER);
}

void test_state_game_over_resume_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_RESUME), GAME_STATE_GAME_OVER);
}

void test_state_game_over_back_to_title_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_BACK_TO_TITLE), GAME_STATE_GAME_OVER);
}

void test_state_game_over_player_died_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_PLAYER_DIED), GAME_STATE_GAME_OVER);
}

void test_state_game_over_run_complete_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_RUN_COMPLETE), GAME_STATE_GAME_OVER);
}

void test_state_game_over_show_highscores_goes_highscores(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_SHOW_HIGHSCORES), GAME_STATE_HIGHSCORES);
}

void test_state_game_over_restart_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_RESTART), GAME_STATE_GAME_OVER);
}

void test_state_game_over_quit_goes_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
}

/* HIGHSCORES exhaustive */

void test_state_highscores_none_stays_highscores(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_NONE), GAME_STATE_HIGHSCORES);
}

void test_state_highscores_start_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_START), GAME_STATE_HIGHSCORES);
}

void test_state_highscores_pause_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_PAUSE), GAME_STATE_HIGHSCORES);
}

void test_state_highscores_resume_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_RESUME), GAME_STATE_HIGHSCORES);
}

void test_state_highscores_back_to_title_goes_title(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_BACK_TO_TITLE), GAME_STATE_TITLE);
}

void test_state_highscores_player_died_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_PLAYER_DIED), GAME_STATE_HIGHSCORES);
}

void test_state_highscores_run_complete_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_RUN_COMPLETE), GAME_STATE_HIGHSCORES);
}

void test_state_highscores_show_highscores_is_noop(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_SHOW_HIGHSCORES), GAME_STATE_HIGHSCORES);
}

void test_state_highscores_restart_goes_running(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_RESTART), GAME_STATE_RUNNING);
}

void test_state_highscores_quit_goes_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
}

/* QUIT exhaustive */

void test_state_quit_none_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_NONE), GAME_STATE_QUIT);
}

void test_state_quit_start_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_START), GAME_STATE_QUIT);
}

void test_state_quit_pause_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_PAUSE), GAME_STATE_QUIT);
}

void test_state_quit_resume_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_RESUME), GAME_STATE_QUIT);
}

void test_state_quit_back_to_title_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_BACK_TO_TITLE), GAME_STATE_QUIT);
}

void test_state_quit_player_died_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_PLAYER_DIED), GAME_STATE_QUIT);
}

void test_state_quit_run_complete_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_RUN_COMPLETE), GAME_STATE_QUIT);
}

void test_state_quit_show_highscores_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_SHOW_HIGHSCORES), GAME_STATE_QUIT);
}

void test_state_quit_restart_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_RESTART), GAME_STATE_QUIT);
}

void test_state_quit_quit_stays_quit(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_QUIT, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
}

/* ------------------------------------------------------------------ */
/* Multi-hop transition chains (full gameplay flows)                   */
/* ------------------------------------------------------------------ */

void test_state_full_play_cycle(void) {
    GameState s = GAME_STATE_BOOT;

    s = state_next(s, TRANSITION_EVENT_NONE);
    ASSERT_EQ_INT(s, GAME_STATE_TITLE);

    s = state_next(s, TRANSITION_EVENT_START);
    ASSERT_EQ_INT(s, GAME_STATE_RUNNING);

    s = state_next(s, TRANSITION_EVENT_PLAYER_DIED);
    ASSERT_EQ_INT(s, GAME_STATE_GAME_OVER);

    s = state_next(s, TRANSITION_EVENT_SHOW_HIGHSCORES);
    ASSERT_EQ_INT(s, GAME_STATE_HIGHSCORES);

    s = state_next(s, TRANSITION_EVENT_RESTART);
    ASSERT_EQ_INT(s, GAME_STATE_RUNNING);
}

void test_state_play_pause_resume_cycle(void) {
    GameState s = GAME_STATE_RUNNING;

    s = state_next(s, TRANSITION_EVENT_PAUSE);
    ASSERT_EQ_INT(s, GAME_STATE_PAUSED);

    s = state_next(s, TRANSITION_EVENT_RESUME);
    ASSERT_EQ_INT(s, GAME_STATE_RUNNING);

    s = state_next(s, TRANSITION_EVENT_PAUSE);
    ASSERT_EQ_INT(s, GAME_STATE_PAUSED);

    s = state_next(s, TRANSITION_EVENT_RESUME);
    ASSERT_EQ_INT(s, GAME_STATE_RUNNING);
}

void test_state_paused_back_to_title_restart_cycle(void) {
    GameState s = GAME_STATE_RUNNING;

    s = state_next(s, TRANSITION_EVENT_PAUSE);
    ASSERT_EQ_INT(s, GAME_STATE_PAUSED);

    s = state_next(s, TRANSITION_EVENT_BACK_TO_TITLE);
    ASSERT_EQ_INT(s, GAME_STATE_TITLE);

    s = state_next(s, TRANSITION_EVENT_START);
    ASSERT_EQ_INT(s, GAME_STATE_RUNNING);
}

void test_state_highscores_back_to_title_cycle(void) {
    GameState s = GAME_STATE_HIGHSCORES;

    s = state_next(s, TRANSITION_EVENT_BACK_TO_TITLE);
    ASSERT_EQ_INT(s, GAME_STATE_TITLE);

    s = state_next(s, TRANSITION_EVENT_START);
    ASSERT_EQ_INT(s, GAME_STATE_RUNNING);
}

void test_state_win_flow_running_to_highscores(void) {
    GameState s = GAME_STATE_RUNNING;

    s = state_next(s, TRANSITION_EVENT_RUN_COMPLETE);
    ASSERT_EQ_INT(s, GAME_STATE_HIGHSCORES);

    s = state_next(s, TRANSITION_EVENT_QUIT);
    ASSERT_EQ_INT(s, GAME_STATE_QUIT);
}

void test_state_quit_from_every_non_boot_state(void) {
    ASSERT_EQ_INT(state_next(GAME_STATE_TITLE, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
    ASSERT_EQ_INT(state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
    ASSERT_EQ_INT(state_next(GAME_STATE_PAUSED, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_QUIT), GAME_STATE_QUIT);
}

/* ------------------------------------------------------------------ */
/* Design doc: transition priority (death > run_complete > pause)      */
/* ------------------------------------------------------------------ */

void test_state_death_priority_over_pause(void) {
    /* When both could occur, PLAYER_DIED takes priority in the design.
       The state machine itself handles one event at a time, so this
       tests that PLAYER_DIED is handled correctly from RUNNING. */
    GameState after_death = state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_PLAYER_DIED);
    GameState after_pause = state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_PAUSE);

    ASSERT_EQ_INT(after_death, GAME_STATE_GAME_OVER);
    ASSERT_EQ_INT(after_pause, GAME_STATE_PAUSED);

    /* Death leads to game_over which cannot be paused */
    ASSERT_EQ_INT(state_next(GAME_STATE_GAME_OVER, TRANSITION_EVENT_PAUSE), GAME_STATE_GAME_OVER);
}

void test_state_run_complete_priority_over_pause(void) {
    GameState after_complete = state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_RUN_COMPLETE);
    ASSERT_EQ_INT(after_complete, GAME_STATE_HIGHSCORES);

    /* Highscores cannot be paused */
    ASSERT_EQ_INT(state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_PAUSE), GAME_STATE_HIGHSCORES);
}

/* Idempotence: applying NONE repeatedly should not change state */
void test_state_idempotence_none_event(void) {
    GameState states[] = {
        GAME_STATE_TITLE, GAME_STATE_RUNNING, GAME_STATE_PAUSED,
        GAME_STATE_GAME_OVER, GAME_STATE_HIGHSCORES, GAME_STATE_QUIT
    };
    int i;

    for (i = 0; i < 6; ++i) {
        GameState before = states[i];
        GameState after = state_next(before, TRANSITION_EVENT_NONE);
        ASSERT_EQ_INT(after, before);
    }
}

/* Repeated transitions are safe */
void test_state_repeated_pause_resume(void) {
    GameState s = GAME_STATE_RUNNING;
    int i;

    for (i = 0; i < 100; ++i) {
        s = state_next(s, TRANSITION_EVENT_PAUSE);
        ASSERT_EQ_INT(s, GAME_STATE_PAUSED);
        s = state_next(s, TRANSITION_EVENT_RESUME);
        ASSERT_EQ_INT(s, GAME_STATE_RUNNING);
    }
}
