#include "snakeman/world.h"

#include <stddef.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* RNG                                                                 */
/* ------------------------------------------------------------------ */

uint32_t rng_next(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/* ------------------------------------------------------------------ */
/* Maze generation: randomized DFS carving                            */
/* ------------------------------------------------------------------ */

static void shuffle4(int *arr, uint32_t *rng) {
    int i;
    for (i = 3; i > 0; --i) {
        int j = (int)(rng_next(rng) % (uint32_t)(i + 1));
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

/* Carve passages using iterative DFS with an explicit stack */
static void maze_carve(Maze *maze, uint32_t seed) {
    /* DFS stack */
    typedef struct { int x, y; } Cell;
    static Cell stack[MAZE_COLS * MAZE_ROWS];
    static uint8_t visited[MAZE_ROWS][MAZE_COLS];
    int top = 0;
    uint32_t rng = seed;
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    memset(visited, 0, sizeof(visited));

    /* Fill all with walls first */
    {
        int r, c;
        for (r = 0; r < MAZE_ROWS; ++r) {
            for (c = 0; c < MAZE_COLS; ++c) {
                maze->tiles[r][c] = TILE_WALL;
            }
        }
    }

    /* Start from (1,1) - using odd coordinates for maze cells */
    {
        int sx = 1, sy = 1;
        maze->tiles[sy][sx] = TILE_FLOOR;
        visited[sy][sx] = 1;
        stack[top].x = sx;
        stack[top].y = sy;
        top++;
    }

    while (top > 0) {
        Cell cur = stack[top - 1];
        int dirs[4] = {0, 1, 2, 3};
        int found = 0;
        int i;

        shuffle4(dirs, &rng);

        for (i = 0; i < 4; ++i) {
            int d = dirs[i];
            int nx = cur.x + dx[d] * 2;
            int ny = cur.y + dy[d] * 2;

            if (nx >= 1 && nx < MAZE_COLS - 1 && ny >= 1 && ny < MAZE_ROWS - 1
                && !visited[ny][nx]) {
                /* Carve through the wall between cur and (nx, ny) */
                maze->tiles[cur.y + dy[d]][cur.x + dx[d]] = TILE_FLOOR;
                maze->tiles[ny][nx] = TILE_FLOOR;
                visited[ny][nx] = 1;
                stack[top].x = nx;
                stack[top].y = ny;
                top++;
                found = 1;
                break;
            }
        }

        if (!found) {
            top--;
        }
    }

    /* Open extra passages to make the maze more navigable */
    {
        int r, c;
        for (r = 2; r < MAZE_ROWS - 2; ++r) {
            for (c = 2; c < MAZE_COLS - 2; ++c) {
                if (maze->tiles[r][c] == TILE_WALL) {
                    /* Check if opening this wall connects two floor tiles */
                    int h_open = (maze->tiles[r][c-1] == TILE_FLOOR && maze->tiles[r][c+1] == TILE_FLOOR);
                    int v_open = (maze->tiles[r-1][c] == TILE_FLOOR && maze->tiles[r+1][c] == TILE_FLOOR);
                    if ((h_open || v_open) && (rng_next(&rng) % 100) < 30) {
                        maze->tiles[r][c] = TILE_FLOOR;
                    }
                }
            }
        }
    }

    /* Ensure borders are walls */
    {
        int i;
        for (i = 0; i < MAZE_COLS; ++i) {
            maze->tiles[0][i] = TILE_WALL;
            maze->tiles[MAZE_ROWS - 1][i] = TILE_WALL;
        }
        for (i = 0; i < MAZE_ROWS; ++i) {
            maze->tiles[i][0] = TILE_WALL;
            maze->tiles[i][MAZE_COLS - 1] = TILE_WALL;
        }
    }

    /* Create spawn area in center (clear a 5x5 zone) */
    {
        int cx = MAZE_COLS / 2;
        int cy = MAZE_ROWS / 2;
        int r, c;
        for (r = cy - 2; r <= cy + 2; ++r) {
            for (c = cx - 2; c <= cx + 2; ++c) {
                if (r > 0 && r < MAZE_ROWS - 1 && c > 0 && c < MAZE_COLS - 1) {
                    maze->tiles[r][c] = TILE_FLOOR;
                }
            }
        }
    }
}

void maze_generate(Maze *maze, uint32_t seed) {
    if (maze == NULL) return;
    maze_carve(maze, seed != 0 ? seed : 12345);
}

int maze_is_walkable(const Maze *maze, int x, int y) {
    if (maze == NULL) return 0;
    if (x < 0 || x >= MAZE_COLS || y < 0 || y >= MAZE_ROWS) return 0;
    return maze->tiles[y][x] == TILE_FLOOR;
}

/* ------------------------------------------------------------------ */
/* Direction helpers                                                    */
/* ------------------------------------------------------------------ */

static int dir_dx(Direction d) {
    static const int dxs[] = {0, 1, 0, -1};
    return dxs[d & 3];
}

static int dir_dy(Direction d) {
    static const int dys[] = {-1, 0, 1, 0};
    return dys[d & 3];
}

static Direction dir_opposite(Direction d) {
    return (Direction)((d + 2) & 3);
}

/* ------------------------------------------------------------------ */
/* Player                                                              */
/* ------------------------------------------------------------------ */

void player_init(Player *p, GridPos start, Direction dir) {
    if (p == NULL) return;
    memset(p, 0, sizeof(*p));
    p->segments[0] = start;
    p->seg_dirs[0] = dir;
    p->length = 1;
    p->dir = dir;
    p->queued_dir = dir;
    p->grow_pending = 2; /* start with 3 segments total */
    p->alive = 1;
    p->has_shield = 0;
    p->base_move_interval = 0.15f;
    p->move_interval = p->base_move_interval;
    p->move_timer = 0.0f;
}

static void player_move(Player *p, const Maze *maze) {
    GridPos new_head;
    int nx, ny;
    int i;

    /* Try queued direction first */
    nx = p->segments[0].x + dir_dx(p->queued_dir);
    ny = p->segments[0].y + dir_dy(p->queued_dir);
    if (maze_is_walkable(maze, nx, ny)) {
        p->dir = p->queued_dir;
    }

    /* Move in current direction */
    new_head.x = p->segments[0].x + dir_dx(p->dir);
    new_head.y = p->segments[0].y + dir_dy(p->dir);

    if (!maze_is_walkable(maze, new_head.x, new_head.y)) {
        /* Can't move forward, stay put */
        return;
    }

    /* Shift segments */
    if (p->grow_pending > 0 && p->length < MAX_SNAKE_LEN) {
        /* Grow: don't remove tail */
        for (i = p->length; i > 0; --i) {
            p->segments[i] = p->segments[i - 1];
            p->seg_dirs[i] = p->seg_dirs[i - 1];
        }
        p->length++;
        p->grow_pending--;
    } else {
        /* Normal move: shift body, drop tail */
        for (i = p->length - 1; i > 0; --i) {
            p->segments[i] = p->segments[i - 1];
            p->seg_dirs[i] = p->seg_dirs[i - 1];
        }
    }

    p->segments[0] = new_head;
    p->seg_dirs[0] = p->dir;
}

/* ------------------------------------------------------------------ */
/* Robot                                                                */
/* ------------------------------------------------------------------ */

void robot_init(Robot *r, GridPos start) {
    if (r == NULL) return;
    memset(r, 0, sizeof(*r));
    r->pos = start;
    r->dir = DIR_UP;
    r->active = 1;
    r->base_move_interval = 0.22f;
    r->move_interval = r->base_move_interval;
    r->move_timer = 0.0f;
    r->last_dir = DIR_UP;
}

static int manhattan_dist(GridPos a, GridPos b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx + dy;
}

static void robot_chase(Robot *r, const Maze *maze, GridPos target) {
    /* Try all 4 directions, pick the one that reduces manhattan distance most.
       Avoid reversing direction when possible. */
    Direction best = r->dir;
    int best_dist = 9999;
    Direction opp = dir_opposite(r->last_dir);
    int tried_any = 0;
    int d;

    for (d = 0; d < 4; ++d) {
        Direction dd = (Direction)d;
        int nx = r->pos.x + dir_dx(dd);
        int ny = r->pos.y + dir_dy(dd);
        GridPos np;
        int dist;

        if (!maze_is_walkable(maze, nx, ny)) continue;
        /* Avoid immediate reversal unless no other option */
        if (dd == opp && tried_any) continue;

        np.x = nx;
        np.y = ny;
        dist = manhattan_dist(np, target);

        if (dist < best_dist || !tried_any) {
            best_dist = dist;
            best = dd;
            tried_any = 1;
        }
    }

    /* If best is the opposite (only option), allow it */
    if (!tried_any) {
        /* Stuck, try opposite */
        int nx = r->pos.x + dir_dx(opp);
        int ny = r->pos.y + dir_dy(opp);
        if (maze_is_walkable(maze, nx, ny)) {
            best = opp;
        } else {
            return; /* truly stuck */
        }
    }

    {
        int nx = r->pos.x + dir_dx(best);
        int ny = r->pos.y + dir_dy(best);
        if (maze_is_walkable(maze, nx, ny)) {
            r->last_dir = best;
            r->dir = best;
            r->pos.x = nx;
            r->pos.y = ny;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Find a walkable position                                            */
/* ------------------------------------------------------------------ */

static GridPos find_walkable(const Maze *maze, uint32_t *rng, int avoid_cx, int avoid_cy, int min_dist) {
    GridPos p;
    int attempts = 0;
    do {
        p.x = 1 + (int)(rng_next(rng) % (uint32_t)(MAZE_COLS - 2));
        p.y = 1 + (int)(rng_next(rng) % (uint32_t)(MAZE_ROWS - 2));
        attempts++;
        if (attempts > 2000) break;
    } while (!maze_is_walkable(maze, p.x, p.y)
             || manhattan_dist(p, (GridPos){avoid_cx, avoid_cy}) < min_dist);
    return p;
}

/* ------------------------------------------------------------------ */
/* World                                                               */
/* ------------------------------------------------------------------ */

void world_init(World *w, uint32_t seed) {
    int i;
    GridPos center;
    GridPos robot_corners[4];

    if (w == NULL) return;
    memset(w, 0, sizeof(*w));

    w->rng_state = seed != 0 ? seed : 42;
    w->robot_speed_scale = 1.0f;
    w->difficulty_milestone = 0;
    w->combo_window_shrink = 0.0f;

    maze_generate(&w->maze, seed);

    /* Player starts at center */
    center.x = MAZE_COLS / 2;
    center.y = MAZE_ROWS / 2;
    player_init(&w->player, center, DIR_RIGHT);

    /* Robots start near corners */
    robot_corners[0].x = 3;            robot_corners[0].y = 3;
    robot_corners[1].x = MAZE_COLS-4;  robot_corners[1].y = 3;
    robot_corners[2].x = 3;            robot_corners[2].y = MAZE_ROWS-4;
    robot_corners[3].x = MAZE_COLS-4;  robot_corners[3].y = MAZE_ROWS-4;

    for (i = 0; i < NUM_ROBOTS; ++i) {
        GridPos rp = robot_corners[i];
        /* Nudge to nearest walkable */
        if (!maze_is_walkable(&w->maze, rp.x, rp.y)) {
            rp = find_walkable(&w->maze, &w->rng_state, center.x, center.y, 5);
        }
        robot_init(&w->robots[i], rp);
        /* Stagger robot start so they don't all move at once */
        w->robots[i].move_timer = (float)i * 0.05f;
    }

    /* Place pickups on walkable tiles */
    w->pickup_count = 0;
    {
        int r, c;
        for (r = 1; r < MAZE_ROWS - 1 && w->pickup_count < MAX_PICKUPS; ++r) {
            for (c = 1; c < MAZE_COLS - 1 && w->pickup_count < MAX_PICKUPS; ++c) {
                if (maze_is_walkable(&w->maze, c, r)) {
                    /* Skip center spawn area */
                    int dcx = c - center.x;
                    int dcy = r - center.y;
                    if (dcx < 0) dcx = -dcx;
                    if (dcy < 0) dcy = -dcy;
                    if (dcx <= 2 && dcy <= 2) continue;

                    /* Place pickup with some probability */
                    if ((rng_next(&w->rng_state) % 100) < 12) {
                        w->pickups[w->pickup_count].pos.x = c;
                        w->pickups[w->pickup_count].pos.y = r;
                        w->pickups[w->pickup_count].active = 1;
                        w->pickup_count++;
                    }
                }
            }
        }
    }
    w->pickups_total = w->pickup_count;

    /* Place a couple of powerups */
    w->powerup_count = 0;
    for (i = 0; i < 3 && w->powerup_count < MAX_POWERUPS; ++i) {
        GridPos pp = find_walkable(&w->maze, &w->rng_state, center.x, center.y, 6);
        w->powerups[w->powerup_count].pos = pp;
        w->powerups[w->powerup_count].type = (PowerupType)(1 + (int)(rng_next(&w->rng_state) % 5));
        w->powerups[w->powerup_count].active = 1;
        w->powerup_count++;
    }

    w->active_powerup.type = POWERUP_NONE;
    w->active_powerup.timer = 0.0f;
}

/* Check collision between player head and robots */
static int check_robot_collision(const World *w) {
    int i;
    GridPos head = w->player.segments[0];
    for (i = 0; i < NUM_ROBOTS; ++i) {
        if (w->robots[i].active && w->robots[i].pos.x == head.x && w->robots[i].pos.y == head.y) {
            return 1;
        }
    }
    return 0;
}

/* Check collision between player head and own body segments.
   Only meaningful when length > 1.  Shield does NOT protect. */
static int check_self_collision(const Player *p) {
    int i;
    if (p->length <= 1) return 0;
    for (i = 1; i < p->length; ++i) {
        if (p->segments[0].x == p->segments[i].x &&
            p->segments[0].y == p->segments[i].y) {
            return 1;
        }
    }
    return 0;
}

/* Apply dynamic difficulty at 25% milestones.
   Per design doc: robot speed +5%, combo timer shrinks 0.2s (floor 2.2s). */
static void check_difficulty(World *w, int pickups_collected) {
    int threshold;
    int i;

    if (w->pickups_total <= 0) return;
    threshold = (w->pickups_total * (w->difficulty_milestone + 1)) / 4;

    if (pickups_collected >= threshold && w->difficulty_milestone < 4) {
        w->difficulty_milestone++;
        w->robot_speed_scale += 0.05f;

        /* Cap robot speed: robot interval must be >= player interval / 0.92 */
        for (i = 0; i < NUM_ROBOTS; ++i) {
            float new_interval = w->robots[i].base_move_interval / w->robot_speed_scale;
            float min_interval = w->player.move_interval / 0.92f;
            if (new_interval < min_interval) new_interval = min_interval;
            w->robots[i].move_interval = new_interval;
        }

        /* Combo timer shrinks by 0.2s per milestone, floor 2.2s */
        w->combo_window_shrink = (float)w->difficulty_milestone * 0.2f;
    }
}

/* Magnet: attract nearby pickups toward the player's path.
   Moves each active pickup one tile closer to the player head per call
   if within MAGNET_RADIUS manhattan distance. */
#define MAGNET_RADIUS 5

static void magnet_attract(World *w) {
    int i;
    GridPos head = w->player.segments[0];
    for (i = 0; i < w->pickup_count; ++i) {
        int dx, dy, adx, ady;
        GridPos np;
        if (!w->pickups[i].active) continue;
        if (manhattan_dist(w->pickups[i].pos, head) > MAGNET_RADIUS) continue;
        /* Already on player head — will be collected this tick */
        if (w->pickups[i].pos.x == head.x && w->pickups[i].pos.y == head.y) continue;

        dx = head.x - w->pickups[i].pos.x;
        dy = head.y - w->pickups[i].pos.y;
        adx = dx < 0 ? -dx : dx;
        ady = dy < 0 ? -dy : dy;

        /* Move one step along the axis with greater distance */
        np = w->pickups[i].pos;
        if (adx >= ady) {
            np.x += (dx > 0) ? 1 : -1;
        } else {
            np.y += (dy > 0) ? 1 : -1;
        }

        /* Only move if target tile is walkable */
        if (maze_is_walkable(&w->maze, np.x, np.y)) {
            w->pickups[i].pos = np;
        }
    }
}

int world_update(World *w, Direction input_dir, float dt) {
    int i;
    int events = WORLD_EVENT_NONE;
    GridPos head;
    int robot_nearby;
    float emp_slow;

    if (w == NULL || !w->player.alive) return WORLD_EVENT_NONE;

    /* Buffer input direction */
    if (input_dir != (Direction)(-1)) {
        /* Don't allow 180-degree reversal */
        if (w->player.length <= 1 || input_dir != dir_opposite(w->player.dir)) {
            w->player.queued_dir = input_dir;
        }
    }

    /* Update active powerup */
    if (w->active_powerup.type != POWERUP_NONE) {
        /* Shield is consumed on hit, not by timer */
        if (w->active_powerup.type != POWERUP_SHIELD) {
            w->active_powerup.timer -= dt;
            if (w->active_powerup.timer <= 0.0f) {
                /* Deactivate powerup effects */
                if (w->active_powerup.type == POWERUP_OVERCLOCK) {
                    w->player.move_interval = w->player.base_move_interval;
                }
                w->active_powerup.type = POWERUP_NONE;
                w->active_powerup.timer = 0.0f;
            }
        }
    }

    emp_slow = (w->active_powerup.type == POWERUP_EMP) ? 1.4f : 1.0f;

    /* Magnet attraction: pull nearby pickups toward player once per player step */
    if (w->active_powerup.type == POWERUP_MAGNET) {
        magnet_attract(w);
    }

    /* Player movement */
    w->player.move_timer += dt;
    if (w->player.move_timer >= w->player.move_interval) {
        w->player.move_timer -= w->player.move_interval;
        player_move(&w->player, &w->maze);
    }

    /* Robot movement */
    for (i = 0; i < NUM_ROBOTS; ++i) {
        if (!w->robots[i].active) continue;
        w->robots[i].move_timer += dt;
        if (w->robots[i].move_timer >= w->robots[i].move_interval * emp_slow) {
            w->robots[i].move_timer -= w->robots[i].move_interval * emp_slow;
            robot_chase(&w->robots[i], &w->maze, w->player.segments[0]);
        }
    }

    /* Check robot collision */
    if (check_robot_collision(w)) {
        if (w->player.has_shield) {
            w->player.has_shield = 0;
            /* Shield consumed — deactivate the shield powerup */
            if (w->active_powerup.type == POWERUP_SHIELD) {
                w->active_powerup.type = POWERUP_NONE;
                w->active_powerup.timer = 0.0f;
            }
        } else {
            w->player.alive = 0;
            return WORLD_EVENT_PLAYER_DIED;
        }
    }

    /* Check self-collision (shield does NOT protect) */
    if (check_self_collision(&w->player)) {
        w->player.alive = 0;
        return WORLD_EVENT_PLAYER_DIED;
    }

    /* Check pickup collection */
    head = w->player.segments[0];
    robot_nearby = 0;
    for (i = 0; i < NUM_ROBOTS; ++i) {
        if (w->robots[i].active && manhattan_dist(head, w->robots[i].pos) <= 4) {
            robot_nearby = 1;
            break;
        }
    }

    for (i = 0; i < w->pickup_count; ++i) {
        if (w->pickups[i].active && w->pickups[i].pos.x == head.x && w->pickups[i].pos.y == head.y) {
            w->pickups[i].active = 0;
            w->player.grow_pending++;
            events |= WORLD_EVENT_PICKUP;

            if (robot_nearby) {
                events |= WORLD_EVENT_RISK_PICKUP;
            }
            if (w->active_powerup.type == POWERUP_MAGNET) {
                events |= WORLD_EVENT_MAGNET_PICKUP;
            }
            if (w->active_powerup.type == POWERUP_EMP) {
                events |= WORLD_EVENT_EMP_PICKUP;
            }
        }
    }

    /* Check powerup collection */
    for (i = 0; i < w->powerup_count; ++i) {
        if (w->powerups[i].active && w->powerups[i].pos.x == head.x && w->powerups[i].pos.y == head.y) {
            w->powerups[i].active = 0;
            w->active_powerup.type = w->powerups[i].type;

            switch (w->powerups[i].type) {
                case POWERUP_MAGNET:
                    w->active_powerup.timer = 8.0f;
                    break;
                case POWERUP_EMP:
                    w->active_powerup.timer = 5.0f;
                    break;
                case POWERUP_SHIELD:
                    w->player.has_shield = 1;
                    /* Shield has no timer — lasts until consumed by hit */
                    w->active_powerup.timer = 0.0f;
                    break;
                case POWERUP_OVERCLOCK:
                    w->active_powerup.timer = 6.0f;
                    w->player.move_interval = w->player.base_move_interval * 0.8f;
                    break;
                case POWERUP_MULTIPLIER_ORB:
                    w->active_powerup.timer = 5.0f;
                    break;
                default:
                    break;
            }

            events |= WORLD_EVENT_POWERUP;
        }
    }

    /* Check if all pickups cleared */
    {
        int remaining = 0;
        for (i = 0; i < w->pickup_count; ++i) {
            if (w->pickups[i].active) remaining++;
        }
        if (remaining == 0 && w->pickup_count > 0) {
            /* Include unused-shield bonus flag via ALL_CLEARED */
            return events | WORLD_EVENT_ALL_CLEARED;
        }

        /* Dynamic difficulty */
        {
            int total_collected = w->pickups_total - remaining;
            check_difficulty(w, total_collected);
        }
    }

    return events;
}
