#include "snakeman/state.h"

GameState state_next(GameState current, TransitionEvent event) {
    switch (current) {
        case GAME_STATE_BOOT:
            if (event == TRANSITION_EVENT_NONE) {
                return GAME_STATE_TITLE;
            }
            if (event == TRANSITION_EVENT_QUIT) {
                return GAME_STATE_QUIT;
            }
            return current;
        case GAME_STATE_TITLE:
            if (event == TRANSITION_EVENT_START) {
                return GAME_STATE_RUNNING;
            }
            if (event == TRANSITION_EVENT_QUIT) {
                return GAME_STATE_QUIT;
            }
            return current;
        case GAME_STATE_RUNNING:
            if (event == TRANSITION_EVENT_PLAYER_DIED) {
                return GAME_STATE_GAME_OVER;
            }
            if (event == TRANSITION_EVENT_RUN_COMPLETE) {
                return GAME_STATE_HIGHSCORES;
            }
            if (event == TRANSITION_EVENT_PAUSE) {
                return GAME_STATE_PAUSED;
            }
            if (event == TRANSITION_EVENT_QUIT) {
                return GAME_STATE_QUIT;
            }
            return current;
        case GAME_STATE_PAUSED:
            if (event == TRANSITION_EVENT_RESUME) {
                return GAME_STATE_RUNNING;
            }
            if (event == TRANSITION_EVENT_BACK_TO_TITLE) {
                return GAME_STATE_TITLE;
            }
            if (event == TRANSITION_EVENT_QUIT) {
                return GAME_STATE_QUIT;
            }
            return current;
        case GAME_STATE_GAME_OVER:
            if (event == TRANSITION_EVENT_SHOW_HIGHSCORES) {
                return GAME_STATE_HIGHSCORES;
            }
            if (event == TRANSITION_EVENT_QUIT) {
                return GAME_STATE_QUIT;
            }
            return current;
        case GAME_STATE_HIGHSCORES:
            if (event == TRANSITION_EVENT_RESTART) {
                return GAME_STATE_RUNNING;
            }
            if (event == TRANSITION_EVENT_BACK_TO_TITLE) {
                return GAME_STATE_TITLE;
            }
            if (event == TRANSITION_EVENT_QUIT) {
                return GAME_STATE_QUIT;
            }
            return current;
        case GAME_STATE_QUIT:
        default:
            return GAME_STATE_QUIT;
    }
}
