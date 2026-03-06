#include "test_framework.h"

#include "snakeman/world.h"

#include <string.h>

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

/* Build a minimal world with an open corridor for controlled testing.
   Clears the entire maze to floor so movement is unrestricted,
   then re-walls the border.  Robots are deactivated so they don't
   interfere with self-collision checks. */
static void setup_open_world(World *w) {
    int r, c, i;
    memset(w, 0, sizeof(*w));

    /* All floor */
    for (r = 0; r < MAZE_ROWS; ++r)
        for (c = 0; c < MAZE_COLS; ++c)
            w->maze.tiles[r][c] = TILE_FLOOR;

    /* Border walls */
    for (c = 0; c < MAZE_COLS; ++c) {
        w->maze.tiles[0][c] = TILE_WALL;
        w->maze.tiles[MAZE_ROWS - 1][c] = TILE_WALL;
    }
    for (r = 0; r < MAZE_ROWS; ++r) {
        w->maze.tiles[r][0] = TILE_WALL;
        w->maze.tiles[r][MAZE_COLS - 1] = TILE_WALL;
    }

    /* Place player at (10, 10) heading right, length 1, no pending growth */
    {
        GridPos start = {10, 10};
        player_init(&w->player, start, DIR_RIGHT);
        w->player.grow_pending = 0;  /* override default; tests set their own */
    }

    /* Deactivate all robots */
    for (i = 0; i < NUM_ROBOTS; ++i) {
        w->robots[i].active = 0;
    }

    w->rng_state = 42;
    w->robot_speed_scale = 1.0f;
    w->pickup_count = 0;
    w->pickups_total = 0;
    w->powerup_count = 0;
    w->active_powerup.type = POWERUP_NONE;
    w->active_powerup.timer = 0.0f;
    w->difficulty_milestone = 0;
    w->combo_window_shrink = 0.0f;
}

/* Force the player to have a specific body layout.
   Caller provides segments array and length.  The direction fields
   are set uniformly to `dir`. */
static void set_player_body(Player *p, const GridPos *segs, int len, Direction dir) {
    int i;
    p->length = len;
    p->dir = dir;
    p->queued_dir = dir;
    p->grow_pending = 0;
    for (i = 0; i < len; ++i) {
        p->segments[i] = segs[i];
        p->seg_dirs[i] = dir;
    }
}

/* Step the world enough times for the player to move one tile.
   Uses a dt equal to the player's move interval so a single call
   triggers exactly one movement step.  Returns the event bitmask. */
static int step_one_move(World *w, Direction input_dir) {
    return world_update(w, input_dir, w->player.move_interval);
}

/* ------------------------------------------------------------------ */
/* Tests: single-segment snake cannot self-collide                    */
/* ------------------------------------------------------------------ */

void test_self_collision_single_segment_no_death(void) {
    World w;
    int events;
    setup_open_world(&w);
    /* Length 1, move right — should never self-collide */
    events = step_one_move(&w, DIR_RIGHT);
    ASSERT_EQ_INT(events & WORLD_EVENT_PLAYER_DIED, 0);
    ASSERT_EQ_INT(w.player.alive, 1);
}

/* ------------------------------------------------------------------ */
/* Tests: multi-segment snake moving straight does NOT self-collide   */
/* ------------------------------------------------------------------ */

void test_self_collision_straight_line_no_death(void) {
    World w;
    int events;
    int i;
    setup_open_world(&w);

    /* Create a 4-segment snake heading right: (10,10) (9,10) (8,10) (7,10) */
    {
        GridPos segs[] = {{10,10},{9,10},{8,10},{7,10}};
        set_player_body(&w.player, segs, 4, DIR_RIGHT);
    }

    /* Step several times going right — no collision expected */
    for (i = 0; i < 5; ++i) {
        events = step_one_move(&w, DIR_RIGHT);
        ASSERT_EQ_INT(events & WORLD_EVENT_PLAYER_DIED, 0);
        ASSERT_EQ_INT(w.player.alive, 1);
    }
}

/* ------------------------------------------------------------------ */
/* Tests: head running into own body causes death                     */
/* ------------------------------------------------------------------ */

void test_self_collision_head_hits_body_causes_death(void) {
    World w;
    int events;
    setup_open_world(&w);

    /* Arrange a snake shaped like:
         . . H . .
         . . 1 . .
         . . 2 . .
         . 4 3 . .
       Head at (10,5), body going down then left:
         segments[0]=(10,5) head, dir=UP
         segments[1]=(10,6)
         segments[2]=(10,7)
         segments[3]=(9,7)
         segments[4]=(9,6)
         segments[5]=(9,5)

       If head moves UP to (10,4) that's fine — no body there.
       Instead, set up a scenario where head is forced into body.

       Better approach: create a U-shaped snake where the next
       move goes into the body.

       Snake heading LEFT:
         segments[0]=(10,10) head  -> will move left to (9,10)
         segments[1]=(11,10)
         segments[2]=(11,9)
         segments[3]=(10,9)
         segments[4]=(9,9)
         segments[5]=(9,10)  <-- body occupies target!

       Because grow_pending=0 the tail (9,10) would be removed
       before the head advances... but player_move shifts first
       then places head.  Actually the move logic shifts segments
       and THEN sets segments[0] to new_head.  The old tail is
       overwritten by the shift.  So (9,10) is vacated.

       We need grow_pending > 0 so the tail is NOT removed.
    */
    {
        GridPos segs[] = {
            {10, 10},  /* head */
            {11, 10},
            {11, 9},
            {10, 9},
            {9,  9},
            {9,  10},  /* tail — occupies the cell head will move into */
        };
        set_player_body(&w.player, segs, 6, DIR_LEFT);
        w.player.grow_pending = 1;  /* tail won't retract */
    }

    events = step_one_move(&w, DIR_LEFT);
    ASSERT_TRUE((events & WORLD_EVENT_PLAYER_DIED) != 0);
    ASSERT_EQ_INT(w.player.alive, 0);
}

/* ------------------------------------------------------------------ */
/* Tests: self-collision still fires even with shield active          */
/* ------------------------------------------------------------------ */

void test_self_collision_with_shield_still_kills(void) {
    World w;
    int events;
    setup_open_world(&w);

    /* Same U-shape body as above */
    {
        GridPos segs[] = {
            {10, 10},
            {11, 10},
            {11, 9},
            {10, 9},
            {9,  9},
            {9,  10},
        };
        set_player_body(&w.player, segs, 6, DIR_LEFT);
        w.player.grow_pending = 1;
        w.player.has_shield = 1;
        w.active_powerup.type = POWERUP_SHIELD;
    }

    events = step_one_move(&w, DIR_LEFT);
    ASSERT_TRUE((events & WORLD_EVENT_PLAYER_DIED) != 0);
    ASSERT_EQ_INT(w.player.alive, 0);
}

/* ------------------------------------------------------------------ */
/* Tests: growing snake self-collides when wrapping back on itself    */
/* ------------------------------------------------------------------ */

void test_self_collision_during_growth(void) {
    World w;
    int events;
    setup_open_world(&w);

    /* 4-segment snake heading right, but immediately reverse to left.
       However, 180 reversal is blocked for length > 1.
       Instead, use a U-turn: go up, then left, then down — head runs into body.

       Build a snake heading UP:
         segments[0]=(15,10) head
         segments[1]=(15,11)
         segments[2]=(15,12)
         segments[3]=(15,13)

       Step 1: move UP -> head at (15,9).  grow_pending=1 -> length becomes 5.
       Step 2: steer LEFT -> head at (14,9).  grow_pending=1 -> length becomes 6.
       Step 3: steer DOWN -> head at (14,10).  grow_pending=1 -> length 7.
       Step 4: steer RIGHT -> head at (15,10) — that's now body seg!  DEATH.
    */
    {
        GridPos segs[] = {{15,10},{15,11},{15,12},{15,13}};
        set_player_body(&w.player, segs, 4, DIR_UP);
        w.player.grow_pending = 4;  /* keep growing so body stays */
    }

    events = step_one_move(&w, DIR_UP);     /* head -> (15,9) */
    ASSERT_EQ_INT(events & WORLD_EVENT_PLAYER_DIED, 0);
    ASSERT_EQ_INT(w.player.alive, 1);

    events = step_one_move(&w, DIR_LEFT);   /* head -> (14,9) */
    ASSERT_EQ_INT(events & WORLD_EVENT_PLAYER_DIED, 0);
    ASSERT_EQ_INT(w.player.alive, 1);

    events = step_one_move(&w, DIR_DOWN);   /* head -> (14,10) */
    ASSERT_EQ_INT(events & WORLD_EVENT_PLAYER_DIED, 0);
    ASSERT_EQ_INT(w.player.alive, 1);

    events = step_one_move(&w, DIR_RIGHT);  /* head -> (15,10) = old body */
    ASSERT_TRUE((events & WORLD_EVENT_PLAYER_DIED) != 0);
    ASSERT_EQ_INT(w.player.alive, 0);
}

/* ------------------------------------------------------------------ */
/* Tests: no self-collision when tail retracts in time                */
/* ------------------------------------------------------------------ */

void test_no_self_collision_when_tail_vacates(void) {
    World w;
    int events;
    setup_open_world(&w);

    /* Snake heading LEFT, grow_pending=0 (tail retracts):
         segments[0]=(10,10) head
         segments[1]=(11,10)
         segments[2]=(11,9)
         segments[3]=(10,9)
         segments[4]=(9,9)
         segments[5]=(9,10)

       Head moves left to (9,10) — but tail at (9,10) retracts first.
       Should NOT die. */
    {
        GridPos segs[] = {
            {10, 10},
            {11, 10},
            {11, 9},
            {10, 9},
            {9,  9},
            {9,  10},
        };
        set_player_body(&w.player, segs, 6, DIR_LEFT);
        w.player.grow_pending = 0;  /* tail retracts */
    }

    events = step_one_move(&w, DIR_LEFT);
    ASSERT_EQ_INT(events & WORLD_EVENT_PLAYER_DIED, 0);
    ASSERT_EQ_INT(w.player.alive, 1);
}

/* ------------------------------------------------------------------ */
/* Tests: world_update returns PLAYER_DIED for self-collision and     */
/*        stops further event processing                              */
/* ------------------------------------------------------------------ */

void test_self_collision_stops_further_events(void) {
    World w;
    int events;
    setup_open_world(&w);

    /* Place a pickup at the cell the head is about to move into,
       which also happens to be a body segment.  Death should take
       priority — WORLD_EVENT_PICKUP should NOT be set. */
    {
        GridPos segs[] = {
            {10, 10},
            {11, 10},
            {11, 9},
            {10, 9},
            {9,  9},
            {9,  10},
        };
        set_player_body(&w.player, segs, 6, DIR_LEFT);
        w.player.grow_pending = 1;
    }

    /* Place pickup at (9,10) — the target cell */
    w.pickups[0].pos.x = 9;
    w.pickups[0].pos.y = 10;
    w.pickups[0].active = 1;
    w.pickup_count = 1;
    w.pickups_total = 1;

    events = step_one_move(&w, DIR_LEFT);
    ASSERT_TRUE((events & WORLD_EVENT_PLAYER_DIED) != 0);
    /* Pickup should NOT have been collected because death happened first */
    ASSERT_EQ_INT(events & WORLD_EVENT_PICKUP, 0);
    ASSERT_EQ_INT(w.player.alive, 0);
}

/* ------------------------------------------------------------------ */
/* Tests: two-segment snake cannot self-collide with normal movement  */
/* ------------------------------------------------------------------ */

void test_self_collision_two_segment_turn_no_death(void) {
    World w;
    int events;
    setup_open_world(&w);

    /* Two-segment snake heading right: (10,10) (9,10).
       Turn up — head moves to (10,9), old head becomes segment 1 at (10,10).
       No overlap. */
    {
        GridPos segs[] = {{10,10},{9,10}};
        set_player_body(&w.player, segs, 2, DIR_RIGHT);
        w.player.grow_pending = 0;
    }

    events = step_one_move(&w, DIR_UP);
    ASSERT_EQ_INT(events & WORLD_EVENT_PLAYER_DIED, 0);
    ASSERT_EQ_INT(w.player.alive, 1);
}

/* ------------------------------------------------------------------ */
/* Tests: 180-reversal blocked prevents trivial self-collision        */
/* ------------------------------------------------------------------ */

void test_self_collision_reversal_blocked(void) {
    World w;
    int events;
    int i;
    setup_open_world(&w);

    /* 3-segment snake heading right: (10,10) (9,10) (8,10).
       Input LEFT (opposite) should be ignored; snake keeps going right.
       No self-collision. */
    {
        GridPos segs[] = {{10,10},{9,10},{8,10}};
        set_player_body(&w.player, segs, 3, DIR_RIGHT);
        w.player.grow_pending = 0;
    }

    for (i = 0; i < 3; ++i) {
        events = step_one_move(&w, DIR_LEFT);  /* should be ignored */
        ASSERT_EQ_INT(events & WORLD_EVENT_PLAYER_DIED, 0);
        ASSERT_EQ_INT(w.player.alive, 1);
    }
    /* Verify the snake actually moved right, not left */
    ASSERT_EQ_INT(w.player.segments[0].x, 13);
    ASSERT_EQ_INT(w.player.segments[0].y, 10);
}
