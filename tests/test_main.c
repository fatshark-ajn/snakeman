#include "test_framework.h"

void test_boot_goes_to_title(void);
void test_running_fatal_event_priority(void);
void test_highscores_restart_goes_running(void);

void test_pickup_increases_score_and_count(void);
void test_combo_expires_back_to_one(void);
void test_highscore_insert_orders_descending(void);

int main(void) {
    RUN_TEST(test_boot_goes_to_title);
    RUN_TEST(test_running_fatal_event_priority);
    RUN_TEST(test_highscores_restart_goes_running);

    RUN_TEST(test_pickup_increases_score_and_count);
    RUN_TEST(test_combo_expires_back_to_one);
    RUN_TEST(test_highscore_insert_orders_descending);

    TEST_SUMMARY();
    return g_test_failures == 0 ? 0 : 1;
}
