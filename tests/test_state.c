#include "test_framework.h"

#include "snakeman/state.h"

void test_boot_goes_to_title(void) {
    GameState next = state_next(GAME_STATE_BOOT, TRANSITION_EVENT_NONE);
    ASSERT_EQ_INT(next, GAME_STATE_TITLE);
}

void test_running_fatal_event_priority(void) {
    GameState next = state_next(GAME_STATE_RUNNING, TRANSITION_EVENT_PLAYER_DIED);
    ASSERT_EQ_INT(next, GAME_STATE_GAME_OVER);
}

void test_highscores_restart_goes_running(void) {
    GameState next = state_next(GAME_STATE_HIGHSCORES, TRANSITION_EVENT_RESTART);
    ASSERT_EQ_INT(next, GAME_STATE_RUNNING);
}
