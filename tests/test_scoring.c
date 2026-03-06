#include "test_framework.h"

#include "snakeman/scoring.h"

void test_pickup_increases_score_and_count(void) {
    ScoreState score;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);

    ASSERT_EQ_INT(score.pickups, 1);
    ASSERT_EQ_INT(score.score, 10);
}

void test_combo_expires_back_to_one(void) {
    ScoreState score;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_TRUE(score.combo_multiplier > 1.0f);

    score_update(&score, 4.0f);
    ASSERT_TRUE(score.combo_multiplier == 1.0f);
}

void test_highscore_insert_orders_descending(void) {
    HighscoreTable table;
    ScoreEntry entry_a;
    ScoreEntry entry_b;
    int rank_a;
    int rank_b;

    highscore_init(&table);

    entry_a.score = 100;
    entry_a.time_sec = 10;
    entry_a.pickups = 5;
    entry_a.max_combo_x100 = 125;
    entry_a.seed = 1;

    entry_b.score = 200;
    entry_b.time_sec = 8;
    entry_b.pickups = 8;
    entry_b.max_combo_x100 = 150;
    entry_b.seed = 2;

    rank_a = highscore_try_insert(&table, entry_a);
    rank_b = highscore_try_insert(&table, entry_b);

    ASSERT_EQ_INT(rank_a, 0);
    ASSERT_EQ_INT(rank_b, 0);
    ASSERT_EQ_INT(table.entries[0].score, 200);
    ASSERT_EQ_INT(table.entries[1].score, 100);
}
