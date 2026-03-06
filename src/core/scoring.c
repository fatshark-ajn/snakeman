#include "snakeman/scoring.h"

#include <stddef.h>

static uint32_t pickup_points(const ScoreState *score) {
    float raw = 10.0f * score->combo_multiplier;
    if (raw < 0.0f) {
        return 0;
    }
    return (uint32_t)(raw + 0.5f);
}

void score_init(ScoreState *score) {
    if (score == NULL) {
        return;
    }

    score->score = 0;
    score->pickups = 0;
    score->combo_timer_sec = 0.0f;
    score->combo_multiplier = 1.0f;
    score->combo_cap = 4.0f;
    score->combo_increment = 0.25f;
    score->combo_default_window_sec = 3.5f;
    score->max_combo_multiplier = 1.0f;
    score->survival_seconds_accum = 0;
}

void score_update(ScoreState *score, float dt) {
    if (score == NULL || dt <= 0.0f) {
        return;
    }

    if (score->combo_timer_sec > 0.0f) {
        score->combo_timer_sec -= dt;
        if (score->combo_timer_sec <= 0.0f) {
            score->combo_timer_sec = 0.0f;
            score->combo_multiplier = 1.0f;
        }
    }
}

void score_on_event(ScoreState *score, ScoreEventType event) {
    if (score == NULL) {
        return;
    }

    switch (event) {
        case SCORE_EVENT_PICKUP:
            score->pickups += 1;
            score->score += pickup_points(score);
            score->combo_timer_sec = score->combo_default_window_sec;
            if (score->combo_multiplier < score->combo_cap) {
                score->combo_multiplier += score->combo_increment;
                if (score->combo_multiplier > score->combo_cap) {
                    score->combo_multiplier = score->combo_cap;
                }
            }
            if (score->combo_multiplier > score->max_combo_multiplier) {
                score->max_combo_multiplier = score->combo_multiplier;
            }
            break;
        case SCORE_EVENT_SURVIVAL_SECOND:
            score->score += 1;
            score->survival_seconds_accum += 1;
            break;
        case SCORE_EVENT_MAGNET_PICKUP:
            score->score += 5;
            break;
        case SCORE_EVENT_RISK_PICKUP:
            score->score += 5;
            break;
        case SCORE_EVENT_NONE:
        default:
            break;
    }
}

ScoreEntry score_finalize_run(const ScoreState *score, uint32_t run_time_sec, uint32_t seed) {
    ScoreEntry entry;

    entry.score = score != NULL ? score->score : 0;
    entry.time_sec = run_time_sec;
    entry.pickups = score != NULL ? score->pickups : 0;
    entry.max_combo_x100 = score != NULL ? (uint8_t)((score->max_combo_multiplier * 100.0f) + 0.5f) : 100;
    entry.seed = seed;

    return entry;
}

void highscore_init(HighscoreTable *table) {
    size_t i;
    if (table == NULL) {
        return;
    }
    for (i = 0; i < 10; ++i) {
        table->entries[i].score = 0;
        table->entries[i].time_sec = 0;
        table->entries[i].pickups = 0;
        table->entries[i].max_combo_x100 = 0;
        table->entries[i].seed = 0;
    }
}

int highscore_try_insert(HighscoreTable *table, ScoreEntry entry) {
    size_t i;

    if (table == NULL) {
        return -1;
    }

    for (i = 0; i < 10; ++i) {
        if (entry.score > table->entries[i].score) {
            size_t j;
            for (j = 9; j > i; --j) {
                table->entries[j] = table->entries[j - 1];
            }
            table->entries[i] = entry;
            return (int)i;
        }
    }

    return -1;
}
