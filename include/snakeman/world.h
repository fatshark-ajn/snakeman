#ifndef SNAKEMAN_WORLD_H
#define SNAKEMAN_WORLD_H

#include <stdint.h>

/* Grid dimensions: 1280/32=40 cols, 720/32=22.5 -> use 22 rows for maze + HUD area */
#define MAZE_COLS 40
#define MAZE_ROWS 22
#define TILE_SIZE 32

#define MAX_SNAKE_LEN 256
#define NUM_ROBOTS 4
#define MAX_PICKUPS 64
#define MAX_POWERUPS 8

/* Tile types */
#define TILE_FLOOR 0
#define TILE_WALL  1

/* Direction enum for grid movement */
typedef enum {
    DIR_UP = 0,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT
} Direction;

/* 2D grid coordinate */
typedef struct {
    int x;
    int y;
} GridPos;

/* ------------------------------------------------------------------ */
/* Maze                                                                */
/* ------------------------------------------------------------------ */

typedef struct {
    uint8_t tiles[MAZE_ROWS][MAZE_COLS];
} Maze;

/* Simple xorshift32 PRNG */
uint32_t rng_next(uint32_t *state);

/* Generate a connected maze with the given seed */
void maze_generate(Maze *maze, uint32_t seed);

/* Query whether a position is walkable */
int maze_is_walkable(const Maze *maze, int x, int y);

/* ------------------------------------------------------------------ */
/* Player (Snakeman)                                                   */
/* ------------------------------------------------------------------ */

typedef struct {
    GridPos segments[MAX_SNAKE_LEN];
    Direction seg_dirs[MAX_SNAKE_LEN];  /* direction each segment is facing */
    int length;
    Direction dir;           /* current movement direction */
    Direction queued_dir;    /* buffered next direction */
    int grow_pending;        /* segments to grow */
    int alive;
    int has_shield;
    float move_timer;
    float move_interval;     /* seconds per grid step */
    float base_move_interval;
} Player;

void player_init(Player *p, GridPos start, Direction dir);

/* ------------------------------------------------------------------ */
/* Robot enemy                                                         */
/* ------------------------------------------------------------------ */

typedef struct {
    GridPos pos;
    Direction dir;
    int active;
    float move_timer;
    float move_interval;
    float base_move_interval;
    Direction last_dir;      /* to avoid immediate reversal */
} Robot;

void robot_init(Robot *r, GridPos start);

/* ------------------------------------------------------------------ */
/* Pickups and powerups                                                */
/* ------------------------------------------------------------------ */

typedef enum {
    POWERUP_NONE = 0,
    POWERUP_MAGNET,
    POWERUP_EMP,
    POWERUP_SHIELD,
    POWERUP_OVERCLOCK,
    POWERUP_MULTIPLIER_ORB
} PowerupType;

typedef struct {
    GridPos pos;
    int active;
} Pickup;

typedef struct {
    GridPos pos;
    PowerupType type;
    int active;
} Powerup;

/* Active powerup state */
typedef struct {
    PowerupType type;
    float timer;
} ActivePowerup;

/* ------------------------------------------------------------------ */
/* World: aggregates all gameplay entities                             */
/* ------------------------------------------------------------------ */

typedef struct {
    Maze maze;
    Player player;
    Robot robots[NUM_ROBOTS];
    Pickup pickups[MAX_PICKUPS];
    int pickup_count;
    int pickups_total;          /* total placed at start */
    Powerup powerups[MAX_POWERUPS];
    int powerup_count;
    ActivePowerup active_powerup;
    uint32_t rng_state;
    int difficulty_milestone;   /* 0-4, tracks 25% thresholds */
    float robot_speed_scale;    /* multiplier on robot speed */
    float combo_window_shrink;  /* total combo window reduction from difficulty */
} World;

/* Initialize the world for a new run */
void world_init(World *w, uint32_t seed);

/* Advance world simulation by one fixed tick.
   Returns a bitmask of WORLD_EVENT_* flags. */
int world_update(World *w, Direction input_dir, float dt);

/* Game events returned by world_update (bitmask) */
#define WORLD_EVENT_NONE            0
#define WORLD_EVENT_PICKUP          (1 << 0)
#define WORLD_EVENT_POWERUP         (1 << 1)
#define WORLD_EVENT_PLAYER_DIED     (1 << 2)
#define WORLD_EVENT_ALL_CLEARED     (1 << 3)
#define WORLD_EVENT_RISK_PICKUP     (1 << 4)
#define WORLD_EVENT_MAGNET_PICKUP   (1 << 5)
#define WORLD_EVENT_EMP_PICKUP      (1 << 6)

#endif
