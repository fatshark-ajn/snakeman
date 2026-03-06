#ifndef SNAKEMAN_SCORING_H
#define SNAKEMAN_SCORING_H

#include <stdint.h>

typedef struct {
    uint32_t score;
    uint32_t time_sec;
    uint16_t pickups;
    uint8_t max_combo_x100;
    uint32_t seed;
} ScoreEntry;

typedef struct {
    ScoreEntry entries[10];
} HighscoreTable;

typedef struct {
    uint32_t score;
    uint16_t pickups;
    float combo_timer_sec;
    float combo_multiplier;
    float combo_cap;
    float combo_increment;
    float combo_default_window_sec;
    float max_combo_multiplier;
    uint32_t survival_seconds_accum;
    uint32_t last_pickup_points;
} ScoreState;

typedef enum {
    SCORE_EVENT_NONE = 0,
    SCORE_EVENT_PICKUP,
    SCORE_EVENT_SURVIVAL_SECOND,
    SCORE_EVENT_MAGNET_PICKUP,
    SCORE_EVENT_RISK_PICKUP
} ScoreEventType;

void score_init(ScoreState *score);
void score_update(ScoreState *score, float dt);
void score_on_event(ScoreState *score, ScoreEventType event);
ScoreEntry score_finalize_run(const ScoreState *score, uint32_t run_time_sec, uint32_t seed);

void highscore_init(HighscoreTable *table);
int highscore_try_insert(HighscoreTable *table, ScoreEntry entry);

#endif
