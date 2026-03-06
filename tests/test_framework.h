#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>

typedef void (*TestFn)(void);

extern int g_test_failures;
extern int g_test_total;

#define ASSERT_TRUE(cond) do { \
    if (!(cond)) { \
        g_test_failures += 1; \
        printf("Assertion failed at %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        return; \
    } \
} while (0)

#define ASSERT_EQ_INT(actual, expected) do { \
    int a_ = (int)(actual); \
    int e_ = (int)(expected); \
    if (a_ != e_) { \
        g_test_failures += 1; \
        printf("Assertion failed at %s:%d: %s (%d) != %s (%d)\n", \
               __FILE__, __LINE__, #actual, a_, #expected, e_); \
        return; \
    } \
} while (0)

#define RUN_TEST(name) do { \
    int before_ = g_test_failures; \
    g_test_total += 1; \
    name(); \
    if (g_test_failures == before_) { \
        printf("[PASS] %s\n", #name); \
    } else { \
        printf("[FAIL] %s\n", #name); \
    } \
} while (0)

#define TEST_SUMMARY() do { \
    printf("\n%d tests, %d failures\n", g_test_total, g_test_failures); \
} while (0)

#endif
