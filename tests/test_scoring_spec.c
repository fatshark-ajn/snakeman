#include "test_framework.h"

#include <stddef.h>

#include "snakeman/scoring.h"

static uint32_t expected_pickup_points(float multiplier) {
    float raw = 10.0f * multiplier;
    if (raw < 0.0f) {
        return 0;
    }
    return (uint32_t)(raw + 0.5f);
}

void test_score_init_matches_design_defaults(void) {
    ScoreState score;

    score_init(&score);

    ASSERT_EQ_INT(score.score, 0);
    ASSERT_EQ_INT(score.pickups, 0);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_cap, 4.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_increment, 0.25f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_default_window_sec, 3.5f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.max_combo_multiplier, 1.0f, 0.0001f);
    ASSERT_EQ_INT(score.survival_seconds_accum, 0);
}

void test_pickup_refreshes_combo_timer_to_default(void) {
    ScoreState score;

    score_init(&score);
    score.combo_timer_sec = 0.4f;

    score_on_event(&score, SCORE_EVENT_PICKUP);

    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, score.combo_default_window_sec, 0.0001f);
}

void test_pickup_uses_multiplier_before_increment(void) {
    ScoreState score;

    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_INT(score.score, 10);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.25f, 0.0001f);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_INT(score.score, 23);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.50f, 0.0001f);
}

void test_combo_multiplier_caps_at_four(void) {
    ScoreState score;
    int i;

    score_init(&score);

    for (i = 0; i < 30; ++i) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
    }

    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 4.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.max_combo_multiplier, 4.0f, 0.0001f);
}

void test_combo_expires_exactly_at_boundary(void) {
    ScoreState score;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);

    score_update(&score, 3.5f);

    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_combo_remains_active_before_window_ends(void) {
    ScoreState score;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);

    score_update(&score, 3.4f);

    ASSERT_TRUE(score.combo_timer_sec > 0.0f);
    ASSERT_TRUE(score.combo_multiplier > 1.0f);
}

void test_survival_event_adds_score_and_seconds(void) {
    ScoreState score;

    score_init(&score);

    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);
    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);
    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);

    ASSERT_EQ_INT(score.score, 3);
    ASSERT_EQ_INT(score.survival_seconds_accum, 3);
}

void test_magnet_pickup_adds_flat_bonus(void) {
    ScoreState score;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);

    ASSERT_EQ_INT(score.score, 5);
}

void test_risk_bonus_is_fifty_percent_of_pickup_value_spec(void) {
    ScoreState score;
    uint32_t before;
    uint32_t awarded;
    uint32_t pickup_value;
    uint32_t expected_bonus;

    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);

    before = score.score;
    pickup_value = expected_pickup_points(score.combo_multiplier);
    expected_bonus = (uint32_t)((pickup_value * 0.5f) + 0.5f);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_RISK_PICKUP);

    awarded = score.score - before;

    ASSERT_EQ_INT((int)awarded, (int)(pickup_value + expected_bonus));
}

void test_score_finalize_run_snapshots_fields(void) {
    ScoreState score;
    ScoreEntry entry;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);

    entry = score_finalize_run(&score, 77, 1234);

    ASSERT_EQ_INT(entry.score, score.score);
    ASSERT_EQ_INT(entry.time_sec, 77);
    ASSERT_EQ_INT(entry.pickups, score.pickups);
    ASSERT_EQ_INT(entry.max_combo_x100, (int)((score.max_combo_multiplier * 100.0f) + 0.5f));
    ASSERT_EQ_INT(entry.seed, 1234);
}

void test_highscore_init_clears_all_entries(void) {
    HighscoreTable table;
    size_t i;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        ASSERT_EQ_INT(table.entries[i].score, 0);
        ASSERT_EQ_INT(table.entries[i].time_sec, 0);
        ASSERT_EQ_INT(table.entries[i].pickups, 0);
        ASSERT_EQ_INT(table.entries[i].max_combo_x100, 0);
        ASSERT_EQ_INT(table.entries[i].seed, 0);
    }
}

void test_highscore_insert_shifts_and_returns_rank(void) {
    HighscoreTable table;
    ScoreEntry e1;
    ScoreEntry e2;
    ScoreEntry e3;
    int r1;
    int r2;
    int r3;

    highscore_init(&table);

    e1.score = 100;
    e1.time_sec = 10;
    e1.pickups = 5;
    e1.max_combo_x100 = 125;
    e1.seed = 1;

    e2.score = 300;
    e2.time_sec = 9;
    e2.pickups = 7;
    e2.max_combo_x100 = 150;
    e2.seed = 2;

    e3.score = 200;
    e3.time_sec = 8;
    e3.pickups = 6;
    e3.max_combo_x100 = 140;
    e3.seed = 3;

    r1 = highscore_try_insert(&table, e1);
    r2 = highscore_try_insert(&table, e2);
    r3 = highscore_try_insert(&table, e3);

    ASSERT_EQ_INT(r1, 0);
    ASSERT_EQ_INT(r2, 0);
    ASSERT_EQ_INT(r3, 1);
    ASSERT_EQ_INT(table.entries[0].score, 300);
    ASSERT_EQ_INT(table.entries[1].score, 200);
    ASSERT_EQ_INT(table.entries[2].score, 100);
}

void test_highscore_rejects_when_score_not_high_enough(void) {
    HighscoreTable table;
    ScoreEntry fill;
    ScoreEntry low;
    int i;
    int rank;

    highscore_init(&table);

    fill.time_sec = 0;
    fill.pickups = 0;
    fill.max_combo_x100 = 100;
    fill.seed = 0;

    for (i = 0; i < 10; ++i) {
        fill.score = (uint32_t)(1000 - i);
        (void)highscore_try_insert(&table, fill);
    }

    low.score = 1;
    low.time_sec = 20;
    low.pickups = 2;
    low.max_combo_x100 = 100;
    low.seed = 42;

    rank = highscore_try_insert(&table, low);

    ASSERT_EQ_INT(rank, -1);
    ASSERT_EQ_INT(table.entries[9].score, 991);
}

void test_highscore_accepts_rank_nine_boundary(void) {
    HighscoreTable table;
    ScoreEntry fill;
    ScoreEntry boundary;
    int i;
    int rank;

    highscore_init(&table);

    fill.time_sec = 0;
    fill.pickups = 0;
    fill.max_combo_x100 = 100;
    fill.seed = 0;

    for (i = 0; i < 9; ++i) {
        fill.score = (uint32_t)(1000 - i);
        (void)highscore_try_insert(&table, fill);
    }

    boundary.score = 10;
    boundary.time_sec = 30;
    boundary.pickups = 1;
    boundary.max_combo_x100 = 100;
    boundary.seed = 9;

    rank = highscore_try_insert(&table, boundary);

    ASSERT_EQ_INT(rank, 9);
    ASSERT_EQ_INT(table.entries[9].score, 10);
}
