#include "test_framework.h"

#include <stddef.h>

#include "snakeman/scoring.h"

/* ------------------------------------------------------------------ */
/* Helper to create a ScoreEntry with given score                      */
/* ------------------------------------------------------------------ */

static ScoreEntry make_entry(uint32_t score, uint32_t time_sec, uint16_t pickups, uint32_t seed) {
    ScoreEntry e;
    e.score = score;
    e.time_sec = time_sec;
    e.pickups = pickups;
    e.max_combo_x100 = 100;
    e.seed = seed;
    return e;
}

/* ------------------------------------------------------------------ */
/* highscore_init                                                      */
/* ------------------------------------------------------------------ */

void test_highscore_init_null_pointer_is_safe(void) {
    highscore_init(NULL);
    ASSERT_TRUE(1);
}

void test_highscore_init_all_fields_zero(void) {
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

void test_highscore_init_can_be_called_twice(void) {
    HighscoreTable table;

    highscore_init(&table);
    (void)highscore_try_insert(&table, make_entry(500, 10, 5, 1));
    ASSERT_EQ_INT(table.entries[0].score, 500);

    highscore_init(&table);
    ASSERT_EQ_INT(table.entries[0].score, 0);
}

/* ------------------------------------------------------------------ */
/* highscore_try_insert: null pointer safety                           */
/* ------------------------------------------------------------------ */

void test_highscore_insert_null_pointer_returns_neg(void) {
    ScoreEntry e = make_entry(100, 10, 5, 1);
    int rank = highscore_try_insert(NULL, e);
    ASSERT_EQ_INT(rank, -1);
}

/* ------------------------------------------------------------------ */
/* highscore_try_insert: basic ordering                                */
/* ------------------------------------------------------------------ */

void test_highscore_insert_single_entry_rank_zero(void) {
    HighscoreTable table;
    int rank;

    highscore_init(&table);
    rank = highscore_try_insert(&table, make_entry(100, 10, 5, 1));

    ASSERT_EQ_INT(rank, 0);
    ASSERT_EQ_INT(table.entries[0].score, 100);
}

void test_highscore_insert_two_ascending_order(void) {
    HighscoreTable table;
    int r1, r2;

    highscore_init(&table);
    r1 = highscore_try_insert(&table, make_entry(100, 10, 5, 1));
    r2 = highscore_try_insert(&table, make_entry(200, 8, 8, 2));

    ASSERT_EQ_INT(r1, 0);
    ASSERT_EQ_INT(r2, 0);
    ASSERT_EQ_INT(table.entries[0].score, 200);
    ASSERT_EQ_INT(table.entries[1].score, 100);
}

void test_highscore_insert_two_descending_order(void) {
    HighscoreTable table;
    int r1, r2;

    highscore_init(&table);
    r1 = highscore_try_insert(&table, make_entry(200, 10, 5, 1));
    r2 = highscore_try_insert(&table, make_entry(100, 8, 8, 2));

    ASSERT_EQ_INT(r1, 0);
    ASSERT_EQ_INT(r2, 1);
    ASSERT_EQ_INT(table.entries[0].score, 200);
    ASSERT_EQ_INT(table.entries[1].score, 100);
}

void test_highscore_insert_three_mixed_order(void) {
    HighscoreTable table;

    highscore_init(&table);
    (void)highscore_try_insert(&table, make_entry(200, 10, 5, 1));
    (void)highscore_try_insert(&table, make_entry(100, 8, 3, 2));
    (void)highscore_try_insert(&table, make_entry(300, 12, 8, 3));

    ASSERT_EQ_INT(table.entries[0].score, 300);
    ASSERT_EQ_INT(table.entries[1].score, 200);
    ASSERT_EQ_INT(table.entries[2].score, 100);
}

/* ------------------------------------------------------------------ */
/* highscore_try_insert: full table behavior                           */
/* ------------------------------------------------------------------ */

void test_highscore_full_table_fill_ten(void) {
    HighscoreTable table;
    int i;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        int rank = highscore_try_insert(&table, make_entry((uint32_t)(100 * (i + 1)), 10, 5, (uint32_t)i));
        ASSERT_TRUE(rank >= 0);
    }

    /* Verify descending order */
    ASSERT_EQ_INT(table.entries[0].score, 1000);
    ASSERT_EQ_INT(table.entries[9].score, 100);
}

void test_highscore_full_table_reject_lower_score(void) {
    HighscoreTable table;
    int i;
    int rank;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)(100 * (10 - i)), 10, 5, (uint32_t)i));
    }

    /* All entries should be 100..1000 descending */
    rank = highscore_try_insert(&table, make_entry(50, 10, 5, 99));
    ASSERT_EQ_INT(rank, -1);
}

void test_highscore_full_table_reject_equal_to_lowest(void) {
    HighscoreTable table;
    int i;
    int rank;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)(100 * (10 - i)), 10, 5, (uint32_t)i));
    }

    /* Lowest is 100, inserting 100 should not displace (not strictly greater) */
    rank = highscore_try_insert(&table, make_entry(100, 10, 5, 99));
    ASSERT_EQ_INT(rank, -1);
}

void test_highscore_full_table_insert_beats_lowest(void) {
    HighscoreTable table;
    int i;
    int rank;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)(100 * (10 - i)), 10, 5, (uint32_t)i));
    }

    /* Lowest is 100, inserting 101 should succeed at rank 9 */
    rank = highscore_try_insert(&table, make_entry(101, 10, 5, 99));
    ASSERT_EQ_INT(rank, 9);
    ASSERT_EQ_INT(table.entries[9].score, 101);
}

void test_highscore_full_table_insert_new_top(void) {
    HighscoreTable table;
    int i;
    int rank;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)(100 * (10 - i)), 10, 5, (uint32_t)i));
    }

    /* Insert new #1 */
    rank = highscore_try_insert(&table, make_entry(5000, 20, 15, 99));
    ASSERT_EQ_INT(rank, 0);
    ASSERT_EQ_INT(table.entries[0].score, 5000);
    /* Old #1 (1000) should now be at rank 1 */
    ASSERT_EQ_INT(table.entries[1].score, 1000);
    /* Old last entry (100) should be pushed out, so entry 9 is now 200 */
    ASSERT_EQ_INT(table.entries[9].score, 200);
}

void test_highscore_full_table_insert_middle(void) {
    HighscoreTable table;
    int i;
    int rank;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)(100 * (10 - i)), 10, 5, (uint32_t)i));
    }

    /* Insert between 500 (rank 5) and 600 (rank 4) -> should be rank 5 */
    rank = highscore_try_insert(&table, make_entry(550, 10, 5, 99));
    ASSERT_EQ_INT(rank, 5);
    ASSERT_EQ_INT(table.entries[5].score, 550);
    ASSERT_EQ_INT(table.entries[4].score, 600);
    ASSERT_EQ_INT(table.entries[6].score, 500);
}

/* ------------------------------------------------------------------ */
/* highscore_try_insert: shift/eviction correctness                    */
/* ------------------------------------------------------------------ */

void test_highscore_shift_preserves_all_fields(void) {
    HighscoreTable table;
    ScoreEntry e;

    highscore_init(&table);

    e.score = 100;
    e.time_sec = 42;
    e.pickups = 7;
    e.max_combo_x100 = 250;
    e.seed = 0xCAFE;

    (void)highscore_try_insert(&table, e);

    /* Insert a higher one, pushing e to rank 1 */
    (void)highscore_try_insert(&table, make_entry(200, 10, 5, 1));

    ASSERT_EQ_INT(table.entries[1].score, 100);
    ASSERT_EQ_INT(table.entries[1].time_sec, 42);
    ASSERT_EQ_INT(table.entries[1].pickups, 7);
    ASSERT_EQ_INT(table.entries[1].max_combo_x100, 250);
    ASSERT_EQ_INT(table.entries[1].seed, (int)0xCAFE);
}

void test_highscore_eviction_drops_rank_ten(void) {
    HighscoreTable table;
    int i;
    uint32_t evicted_seed;

    highscore_init(&table);

    /* Fill with scores 10..1, seed = score value */
    for (i = 0; i < 10; ++i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)(10 - i), 10, 5, (uint32_t)(10 - i)));
    }
    evicted_seed = table.entries[9].seed;

    /* Insert a new top score -- the lowest entry should be evicted */
    (void)highscore_try_insert(&table, make_entry(100, 10, 5, 999));

    ASSERT_EQ_INT(table.entries[0].score, 100);
    /* Entry with seed=evicted_seed should no longer be in the table */
    int found = 0;
    for (i = 0; i < 10; ++i) {
        if (table.entries[i].seed == evicted_seed) {
            found = 1;
        }
    }
    ASSERT_EQ_INT(found, 0);
}

/* ------------------------------------------------------------------ */
/* highscore_try_insert: duplicate scores                              */
/* ------------------------------------------------------------------ */

void test_highscore_insert_identical_scores(void) {
    HighscoreTable table;
    int i;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        int rank = highscore_try_insert(&table, make_entry(100, 10, 5, (uint32_t)i));
        ASSERT_EQ_INT(rank, i);
    }

    /* All entries have score 100 */
    for (i = 0; i < 10; ++i) {
        ASSERT_EQ_INT(table.entries[i].score, 100);
    }
}

void test_highscore_insert_same_score_when_full(void) {
    HighscoreTable table;
    int i;
    int rank;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        (void)highscore_try_insert(&table, make_entry(100, 10, 5, (uint32_t)i));
    }

    /* Table full with all 100s. Inserting 100 should fail (not strictly greater) */
    rank = highscore_try_insert(&table, make_entry(100, 10, 5, 99));
    ASSERT_EQ_INT(rank, -1);
}

void test_highscore_insert_slightly_above_all_same(void) {
    HighscoreTable table;
    int i;
    int rank;

    highscore_init(&table);

    for (i = 0; i < 10; ++i) {
        (void)highscore_try_insert(&table, make_entry(100, 10, 5, (uint32_t)i));
    }

    rank = highscore_try_insert(&table, make_entry(101, 10, 5, 99));
    ASSERT_EQ_INT(rank, 0);
    ASSERT_EQ_INT(table.entries[0].score, 101);
    ASSERT_EQ_INT(table.entries[1].score, 100);
}

/* ------------------------------------------------------------------ */
/* highscore_try_insert: rank return values                            */
/* ------------------------------------------------------------------ */

void test_highscore_insert_ranks_fill_sequentially(void) {
    HighscoreTable table;

    highscore_init(&table);

    /* Insert in descending order -- each should go to the end */
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(1000, 10, 5, 1)), 0);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(900, 10, 5, 2)), 1);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(800, 10, 5, 3)), 2);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(700, 10, 5, 4)), 3);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(600, 10, 5, 5)), 4);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(500, 10, 5, 6)), 5);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(400, 10, 5, 7)), 6);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(300, 10, 5, 8)), 7);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(200, 10, 5, 9)), 8);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(100, 10, 5, 10)), 9);
}

void test_highscore_insert_ranks_ascending_order(void) {
    HighscoreTable table;

    highscore_init(&table);

    /* Insert in ascending order -- each new entry becomes rank 0 */
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(100, 10, 5, 1)), 0);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(200, 10, 5, 2)), 0);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(300, 10, 5, 3)), 0);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(400, 10, 5, 4)), 0);
    ASSERT_EQ_INT(highscore_try_insert(&table, make_entry(500, 10, 5, 5)), 0);
}

/* ------------------------------------------------------------------ */
/* highscore: zero-score entries                                       */
/* ------------------------------------------------------------------ */

void test_highscore_insert_zero_score_into_empty(void) {
    HighscoreTable table;
    int rank;

    highscore_init(&table);

    /* All entries are 0. Zero score is not > 0, so should fail */
    rank = highscore_try_insert(&table, make_entry(0, 10, 5, 1));
    ASSERT_EQ_INT(rank, -1);
}

void test_highscore_insert_one_point_into_empty(void) {
    HighscoreTable table;
    int rank;

    highscore_init(&table);

    rank = highscore_try_insert(&table, make_entry(1, 10, 5, 1));
    ASSERT_EQ_INT(rank, 0);
    ASSERT_EQ_INT(table.entries[0].score, 1);
}

/* ------------------------------------------------------------------ */
/* highscore: stress test -- many inserts                              */
/* ------------------------------------------------------------------ */

void test_highscore_stress_100_inserts(void) {
    HighscoreTable table;
    int i;

    highscore_init(&table);

    for (i = 1; i <= 100; ++i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)i, 10, 5, (uint32_t)i));
    }

    /* Top 10 should be 100 down to 91 */
    ASSERT_EQ_INT(table.entries[0].score, 100);
    ASSERT_EQ_INT(table.entries[1].score, 99);
    ASSERT_EQ_INT(table.entries[9].score, 91);
}

void test_highscore_stress_100_descending_inserts(void) {
    HighscoreTable table;
    int i;

    highscore_init(&table);

    for (i = 100; i >= 1; --i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)i, 10, 5, (uint32_t)i));
    }

    /* Top 10 should be 100 down to 91 */
    ASSERT_EQ_INT(table.entries[0].score, 100);
    ASSERT_EQ_INT(table.entries[1].score, 99);
    ASSERT_EQ_INT(table.entries[9].score, 91);
}

void test_highscore_stress_alternating_inserts(void) {
    HighscoreTable table;
    int i;

    highscore_init(&table);

    /* Insert 1, 100, 2, 99, 3, 98, ... */
    for (i = 0; i < 50; ++i) {
        (void)highscore_try_insert(&table, make_entry((uint32_t)(i + 1), 10, 5, (uint32_t)(i + 1)));
        (void)highscore_try_insert(&table, make_entry((uint32_t)(100 - i), 10, 5, (uint32_t)(100 - i)));
    }

    /* Top 10 should be 100 down to 91 */
    ASSERT_EQ_INT(table.entries[0].score, 100);
    ASSERT_EQ_INT(table.entries[9].score, 91);

    /* Verify full descending order */
    for (i = 0; i < 9; ++i) {
        ASSERT_TRUE(table.entries[i].score >= table.entries[i + 1].score);
    }
}

/* ------------------------------------------------------------------ */
/* highscore: table invariant -- always sorted descending              */
/* ------------------------------------------------------------------ */

void test_highscore_invariant_sorted_descending(void) {
    HighscoreTable table;
    int i;

    highscore_init(&table);

    /* Random-ish insertion pattern */
    (void)highscore_try_insert(&table, make_entry(42, 10, 5, 1));
    (void)highscore_try_insert(&table, make_entry(99, 10, 5, 2));
    (void)highscore_try_insert(&table, make_entry(7, 10, 5, 3));
    (void)highscore_try_insert(&table, make_entry(150, 10, 5, 4));
    (void)highscore_try_insert(&table, make_entry(88, 10, 5, 5));
    (void)highscore_try_insert(&table, make_entry(200, 10, 5, 6));
    (void)highscore_try_insert(&table, make_entry(1, 10, 5, 7));
    (void)highscore_try_insert(&table, make_entry(175, 10, 5, 8));
    (void)highscore_try_insert(&table, make_entry(50, 10, 5, 9));
    (void)highscore_try_insert(&table, make_entry(130, 10, 5, 10));
    (void)highscore_try_insert(&table, make_entry(160, 10, 5, 11));
    (void)highscore_try_insert(&table, make_entry(5, 10, 5, 12));

    for (i = 0; i < 9; ++i) {
        ASSERT_TRUE(table.entries[i].score >= table.entries[i + 1].score);
    }
}
