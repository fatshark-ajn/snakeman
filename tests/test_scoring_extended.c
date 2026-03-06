#include "test_framework.h"

#include <stddef.h>

#include "snakeman/scoring.h"

/* ------------------------------------------------------------------ */
/* score_init: comprehensive defaults verification                    */
/* ------------------------------------------------------------------ */

void test_score_init_null_pointer_is_safe(void) {
    /* Must not crash */
    score_init(NULL);
    ASSERT_TRUE(1);
}

void test_score_init_combo_timer_starts_at_zero(void) {
    ScoreState score;
    score_init(&score);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
}

void test_score_init_multiplier_starts_at_one(void) {
    ScoreState score;
    score_init(&score);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_score_init_can_be_called_twice_safely(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_TRUE(score.score > 0);

    score_init(&score);
    ASSERT_EQ_INT(score.score, 0);
    ASSERT_EQ_INT(score.pickups, 0);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

/* ------------------------------------------------------------------ */
/* score_update: combo timer decay                                     */
/* ------------------------------------------------------------------ */

void test_score_update_null_pointer_is_safe(void) {
    score_update(NULL, 1.0f);
    ASSERT_TRUE(1);
}

void test_score_update_zero_dt_is_noop(void) {
    ScoreState score;
    float timer_before;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    timer_before = score.combo_timer_sec;

    score_update(&score, 0.0f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, timer_before, 0.0001f);
}

void test_score_update_negative_dt_is_noop(void) {
    ScoreState score;
    float timer_before;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    timer_before = score.combo_timer_sec;

    score_update(&score, -1.0f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, timer_before, 0.0001f);
}

void test_score_update_no_active_timer_is_safe(void) {
    ScoreState score;
    score_init(&score);

    /* Timer is at 0 already, update should do nothing harmful */
    score_update(&score, 1.0f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_score_update_timer_decreases_by_dt(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 3.5f, 0.0001f);

    score_update(&score, 1.0f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 2.5f, 0.0001f);
}

void test_score_update_timer_clamps_to_zero(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);

    score_update(&score, 10.0f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
}

void test_score_update_multiplier_resets_on_expire(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_TRUE(score.combo_multiplier > 1.0f);

    score_update(&score, 5.0f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_score_update_multiplier_preserved_if_timer_active(void) {
    ScoreState score;
    float mult;

    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    mult = score.combo_multiplier;

    score_update(&score, 0.5f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, mult, 0.0001f);
}

void test_score_update_multiple_small_steps_equivalent(void) {
    ScoreState a, b;
    int i;

    score_init(&a);
    score_init(&b);

    score_on_event(&a, SCORE_EVENT_PICKUP);
    score_on_event(&b, SCORE_EVENT_PICKUP);

    /* Apply well over 3.5s in small steps to a (use 0.25s steps) */
    for (i = 0; i < 15; ++i) {
        score_update(&a, 0.25f);
    }

    /* Apply 3.75s in one step to b (also well past expiry) */
    score_update(&b, 3.75f);

    /* Both should have expired */
    ASSERT_EQ_FLOAT_NEAR(a.combo_multiplier, 1.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(b.combo_multiplier, 1.0f, 0.0001f);
}

/* ------------------------------------------------------------------ */
/* score_on_event: PICKUP scoring                                      */
/* ------------------------------------------------------------------ */

void test_score_on_event_null_pointer_is_safe(void) {
    score_on_event(NULL, SCORE_EVENT_PICKUP);
    ASSERT_TRUE(1);
}

void test_score_on_event_none_is_noop(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_NONE);
    ASSERT_EQ_INT(score.score, 0);
    ASSERT_EQ_INT(score.pickups, 0);
}

void test_pickup_first_awards_base_ten(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_INT(score.score, 10);
}

void test_pickup_second_awards_at_125x(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10 * 1.0 = 10 */
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10 * 1.25 = 13 (rounded) */
    ASSERT_EQ_INT(score.score, 23);
}

void test_pickup_third_awards_at_150x(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10 */
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 13 -> 23 */
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10*1.5=15 -> 38 */
    ASSERT_EQ_INT(score.score, 38);
}

void test_pickup_fourth_awards_at_175x(void) {
    ScoreState score;
    score_init(&score);
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10 */
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 13 -> 23 */
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 15 -> 38 */
    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10*1.75=18 -> 56 */
    ASSERT_EQ_INT(score.score, 56);
}

void test_pickup_increments_count_correctly(void) {
    ScoreState score;
    int i;
    score_init(&score);

    for (i = 0; i < 7; ++i) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
    }
    ASSERT_EQ_INT(score.pickups, 7);
}

void test_pickup_at_max_multiplier_scores_40(void) {
    ScoreState score;
    int i;
    uint32_t before;
    uint32_t awarded;
    score_init(&score);

    /* Pump multiplier to cap */
    for (i = 0; i < 20; ++i) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
    }
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 4.0f, 0.0001f);

    before = score.score;
    score_on_event(&score, SCORE_EVENT_PICKUP);
    awarded = score.score - before;
    ASSERT_EQ_INT(awarded, 40);
}

/* ------------------------------------------------------------------ */
/* Combo multiplier behavior                                           */
/* ------------------------------------------------------------------ */

void test_combo_multiplier_increments_025_per_pickup(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.25f, 0.0001f);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.50f, 0.0001f);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.75f, 0.0001f);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 2.00f, 0.0001f);
}

void test_combo_multiplier_does_not_exceed_cap(void) {
    ScoreState score;
    int i;
    score_init(&score);

    for (i = 0; i < 50; ++i) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
    }

    ASSERT_TRUE(score.combo_multiplier <= score.combo_cap);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 4.0f, 0.0001f);
}

void test_combo_cap_exactly_at_13th_pickup(void) {
    /* Start at 1.0, +0.25 per pickup -> 13 pickups = 1.0 + 12*0.25 = 4.0 */
    ScoreState score;
    int i;
    score_init(&score);

    for (i = 0; i < 13; ++i) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
    }

    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 4.0f, 0.0001f);
}

void test_combo_timer_refreshes_on_every_pickup(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_update(&score, 3.0f);
    ASSERT_TRUE(score.combo_timer_sec > 0.0f);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 3.5f, 0.0001f);
}

void test_combo_multiplier_reset_then_rebuild(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.5f, 0.0001f);

    /* Expire timer */
    score_update(&score, 4.0f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);

    /* Rebuild */
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.25f, 0.0001f);
}

void test_max_combo_multiplier_tracks_highest(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.max_combo_multiplier, 1.75f, 0.0001f);

    /* Expire and re-check */
    score_update(&score, 4.0f);
    ASSERT_EQ_FLOAT_NEAR(score.max_combo_multiplier, 1.75f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_max_combo_never_decreases(void) {
    ScoreState score;
    int i;
    score_init(&score);

    /* Build to 2.0, expire, build to 1.5 */
    for (i = 0; i < 5; ++i) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
    }
    ASSERT_EQ_FLOAT_NEAR(score.max_combo_multiplier, 2.25f, 0.0001f);

    score_update(&score, 4.0f);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.5f, 0.0001f);

    /* max should still be 2.25 */
    ASSERT_EQ_FLOAT_NEAR(score.max_combo_multiplier, 2.25f, 0.0001f);
}

/* ------------------------------------------------------------------ */
/* Combo timer edge cases                                              */
/* ------------------------------------------------------------------ */

void test_combo_timer_just_barely_active(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_update(&score, 3.4999f);

    ASSERT_TRUE(score.combo_timer_sec > 0.0f);
    ASSERT_TRUE(score.combo_multiplier > 1.0f);
}

void test_combo_timer_exact_expire_resets_multiplier(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_update(&score, 3.5f);

    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_combo_timer_slightly_past_expire(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_update(&score, 3.501f);

    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_combo_timer_does_not_go_negative(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_update(&score, 100.0f);

    ASSERT_TRUE(score.combo_timer_sec >= 0.0f);
}

/* ------------------------------------------------------------------ */
/* Survival tick scoring                                               */
/* ------------------------------------------------------------------ */

void test_survival_single_tick(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);
    ASSERT_EQ_INT(score.score, 1);
    ASSERT_EQ_INT(score.survival_seconds_accum, 1);
}

void test_survival_multiple_ticks(void) {
    ScoreState score;
    int i;
    score_init(&score);

    for (i = 0; i < 60; ++i) {
        score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);
    }
    ASSERT_EQ_INT(score.score, 60);
    ASSERT_EQ_INT(score.survival_seconds_accum, 60);
}

void test_survival_does_not_affect_combo(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
    ASSERT_EQ_INT(score.pickups, 0);
}

void test_survival_does_not_affect_pickup_count(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);
    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);
    ASSERT_EQ_INT(score.pickups, 0);
}

/* ------------------------------------------------------------------ */
/* Magnet pickup scoring                                               */
/* ------------------------------------------------------------------ */

void test_magnet_pickup_awards_flat_five(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);
    ASSERT_EQ_INT(score.score, 5);
}

void test_magnet_pickup_does_not_affect_combo(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_magnet_pickup_does_not_affect_pickup_count(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);
    ASSERT_EQ_INT(score.pickups, 0);
}

void test_magnet_pickup_stacks_with_regular_score(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);        /* +10 */
    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);  /* +5 */
    ASSERT_EQ_INT(score.score, 15);
}

void test_magnet_pickup_multiple_awards_stack(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);
    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);
    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);
    ASSERT_EQ_INT(score.score, 15);
}

/* ------------------------------------------------------------------ */
/* Risk pickup scoring                                                 */
/* ------------------------------------------------------------------ */

void test_risk_pickup_awards_flat_five(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_RISK_PICKUP);
    ASSERT_EQ_INT(score.score, 5);
}

void test_risk_pickup_does_not_affect_combo(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_RISK_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 0.0f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
}

void test_risk_pickup_does_not_affect_pickup_count(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_RISK_PICKUP);
    ASSERT_EQ_INT(score.pickups, 0);
}

void test_risk_pickup_stacks_with_regular_pickup(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);       /* +10 */
    score_on_event(&score, SCORE_EVENT_RISK_PICKUP);   /* +5 */
    ASSERT_EQ_INT(score.score, 15);
}

/* ------------------------------------------------------------------ */
/* Mixed event sequences                                               */
/* ------------------------------------------------------------------ */

void test_mixed_events_survival_and_pickups(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);  /* +1 -> 1 */
    score_on_event(&score, SCORE_EVENT_PICKUP);            /* +10 -> 11 */
    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);  /* +1 -> 12 */
    score_on_event(&score, SCORE_EVENT_PICKUP);            /* +13 -> 25 */

    ASSERT_EQ_INT(score.score, 25);
    ASSERT_EQ_INT(score.pickups, 2);
    ASSERT_EQ_INT(score.survival_seconds_accum, 2);
}

void test_mixed_events_all_types(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);            /* +10 */
    score_on_event(&score, SCORE_EVENT_MAGNET_PICKUP);     /* +5 */
    score_on_event(&score, SCORE_EVENT_RISK_PICKUP);       /* +5 */
    score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);   /* +1 */

    ASSERT_EQ_INT(score.score, 21);
    ASSERT_EQ_INT(score.pickups, 1);
    ASSERT_EQ_INT(score.survival_seconds_accum, 1);
}

void test_long_scoring_session(void) {
    ScoreState score;
    int i;
    score_init(&score);

    /* Simulate a long run: alternate pickups and survival ticks */
    for (i = 0; i < 100; ++i) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
        if (i % 5 == 0) {
            score_on_event(&score, SCORE_EVENT_SURVIVAL_SECOND);
        }
    }

    ASSERT_EQ_INT(score.pickups, 100);
    ASSERT_TRUE(score.score > 0);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 4.0f, 0.0001f);
}

/* ------------------------------------------------------------------ */
/* Combo decay during scoring sequences                                */
/* ------------------------------------------------------------------ */

void test_combo_interleave_pickup_and_decay(void) {
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.25f, 0.0001f);

    score_update(&score, 2.0f);
    ASSERT_TRUE(score.combo_timer_sec > 0.0f);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.5f, 0.0001f);
    ASSERT_EQ_FLOAT_NEAR(score.combo_timer_sec, 3.5f, 0.0001f);
}

void test_combo_decay_across_multiple_expire_cycles(void) {
    ScoreState score;
    int cycle;
    score_init(&score);

    for (cycle = 0; cycle < 5; ++cycle) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
        score_on_event(&score, SCORE_EVENT_PICKUP);
        ASSERT_TRUE(score.combo_multiplier > 1.0f);

        score_update(&score, 4.0f);
        ASSERT_EQ_FLOAT_NEAR(score.combo_multiplier, 1.0f, 0.0001f);
    }

    ASSERT_EQ_INT(score.pickups, 10);
}

/* ------------------------------------------------------------------ */
/* score_finalize_run                                                   */
/* ------------------------------------------------------------------ */

void test_finalize_run_null_pointer_returns_defaults(void) {
    ScoreEntry entry = score_finalize_run(NULL, 42, 999);

    ASSERT_EQ_INT(entry.score, 0);
    ASSERT_EQ_INT(entry.time_sec, 42);
    ASSERT_EQ_INT(entry.pickups, 0);
    ASSERT_EQ_INT(entry.max_combo_x100, 100);
    ASSERT_EQ_INT(entry.seed, 999);
}

void test_finalize_run_captures_score(void) {
    ScoreState score;
    ScoreEntry entry;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);

    entry = score_finalize_run(&score, 10, 1);
    ASSERT_EQ_INT(entry.score, score.score);
}

void test_finalize_run_captures_pickups(void) {
    ScoreState score;
    ScoreEntry entry;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);
    score_on_event(&score, SCORE_EVENT_PICKUP);

    entry = score_finalize_run(&score, 10, 1);
    ASSERT_EQ_INT(entry.pickups, 3);
}

void test_finalize_run_captures_max_combo(void) {
    ScoreState score;
    ScoreEntry entry;
    int i;
    score_init(&score);

    for (i = 0; i < 5; ++i) {
        score_on_event(&score, SCORE_EVENT_PICKUP);
    }
    /* max_combo should be 2.25 */
    score_update(&score, 4.0f); /* expire */

    entry = score_finalize_run(&score, 30, 42);

    /* max_combo_x100 = (2.25 * 100) + 0.5 = 225 */
    ASSERT_EQ_INT(entry.max_combo_x100, 225);
}

void test_finalize_run_captures_seed(void) {
    ScoreState score;
    ScoreEntry entry;
    score_init(&score);

    entry = score_finalize_run(&score, 0, 0xDEADBEEF);
    ASSERT_EQ_INT(entry.seed, (int)0xDEADBEEF);
}

void test_finalize_run_captures_time(void) {
    ScoreState score;
    ScoreEntry entry;
    score_init(&score);

    entry = score_finalize_run(&score, 300, 0);
    ASSERT_EQ_INT(entry.time_sec, 300);
}

void test_finalize_run_with_zero_score(void) {
    ScoreState score;
    ScoreEntry entry;
    score_init(&score);

    entry = score_finalize_run(&score, 0, 0);
    ASSERT_EQ_INT(entry.score, 0);
    ASSERT_EQ_INT(entry.pickups, 0);
    ASSERT_EQ_INT(entry.max_combo_x100, 100);
}

/* ------------------------------------------------------------------ */
/* Score computation accuracy at various multiplier levels             */
/* ------------------------------------------------------------------ */

void test_score_accumulation_exact_sequence(void) {
    /* Verify exact scoring for a known 6-pickup sequence */
    ScoreState score;
    score_init(&score);

    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10*1.00 = 10,  total=10 */
    ASSERT_EQ_INT(score.score, 10);

    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10*1.25 = 13,  total=23 */
    ASSERT_EQ_INT(score.score, 23);

    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10*1.50 = 15,  total=38 */
    ASSERT_EQ_INT(score.score, 38);

    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10*1.75 = 18,  total=56 */
    ASSERT_EQ_INT(score.score, 56);

    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10*2.00 = 20,  total=76 */
    ASSERT_EQ_INT(score.score, 76);

    score_on_event(&score, SCORE_EVENT_PICKUP);  /* 10*2.25 = 23,  total=99 */
    ASSERT_EQ_INT(score.score, 99);
}
