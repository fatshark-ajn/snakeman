#include "snakeman/platform.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* Texture atlas indices                                               */
/* ------------------------------------------------------------------ */

enum {
    TEX_TILE_FLOOR = 0,
    TEX_TILE_WALL,
    TEX_PICKUP_F1,
    TEX_PICKUP_F2,
    TEX_ROBOT_F1,
    TEX_ROBOT_F2,
    TEX_HEAD_UP_F1, TEX_HEAD_UP_F2,
    TEX_HEAD_RIGHT_F1, TEX_HEAD_RIGHT_F2,
    TEX_HEAD_DOWN_F1, TEX_HEAD_DOWN_F2,
    TEX_HEAD_LEFT_F1, TEX_HEAD_LEFT_F2,
    TEX_BODY_H,
    TEX_BODY_V,
    TEX_BODY_TURN_UR,
    TEX_BODY_TURN_RD,
    TEX_BODY_TURN_DL,
    TEX_BODY_TURN_LU,
    TEX_TAIL_UP_F1, TEX_TAIL_UP_F2,
    TEX_TAIL_RIGHT_F1, TEX_TAIL_RIGHT_F2,
    TEX_TAIL_DOWN_F1, TEX_TAIL_DOWN_F2,
    TEX_TAIL_LEFT_F1, TEX_TAIL_LEFT_F2,
    TEX_POWERUP_MAGNET_F1, TEX_POWERUP_MAGNET_F2,
    TEX_POWERUP_EMP_F1, TEX_POWERUP_EMP_F2,
    TEX_POWERUP_SHIELD_F1, TEX_POWERUP_SHIELD_F2,
    TEX_POWERUP_OVERCLOCK_F1, TEX_POWERUP_OVERCLOCK_F2,
    TEX_POWERUP_MULTIPLIER_F1, TEX_POWERUP_MULTIPLIER_F2,
    TEX_UI_TITLE,
    TEX_UI_PAUSED,
    TEX_UI_GAME_OVER,
    TEX_UI_HIGHSCORES,
    TEX_UI_NEW_HIGH,
    TEX_FX_COMBO,
    TEX_FX_RISK,
    TEX_FX_HIT,
    TEX_COUNT
};

typedef struct {
    SDL_Texture *textures[TEX_COUNT];
} Assets;

/* ------------------------------------------------------------------ */
/* Asset loading                                                       */
/* ------------------------------------------------------------------ */

static SDL_Texture *load_tex(SDL_Renderer *r, const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    SDL_Texture *tex = NULL;
    if (surf) {
        tex = SDL_CreateTextureFromSurface(r, surf);
        SDL_FreeSurface(surf);
    }
    if (!tex) {
        fprintf(stderr, "Warning: failed to load %s: %s\n", path, SDL_GetError());
    }
    return tex;
}

static void load_assets(SDL_Renderer *r, Assets *a) {
    memset(a, 0, sizeof(*a));

    a->textures[TEX_TILE_FLOOR] = load_tex(r, "assets/tiles/tile_floor.png");
    a->textures[TEX_TILE_WALL]  = load_tex(r, "assets/tiles/tile_wall.png");

    a->textures[TEX_PICKUP_F1] = load_tex(r, "assets/sprites/pickup_dot_f1.png");
    a->textures[TEX_PICKUP_F2] = load_tex(r, "assets/sprites/pickup_dot_f2.png");
    a->textures[TEX_ROBOT_F1]  = load_tex(r, "assets/sprites/robot_enemy_f1.png");
    a->textures[TEX_ROBOT_F2]  = load_tex(r, "assets/sprites/robot_enemy_f2.png");

    a->textures[TEX_HEAD_UP_F1]    = load_tex(r, "assets/sprites/player/snakeman_head_up_f1.png");
    a->textures[TEX_HEAD_UP_F2]    = load_tex(r, "assets/sprites/player/snakeman_head_up_f2.png");
    a->textures[TEX_HEAD_RIGHT_F1] = load_tex(r, "assets/sprites/player/snakeman_head_right_f1.png");
    a->textures[TEX_HEAD_RIGHT_F2] = load_tex(r, "assets/sprites/player/snakeman_head_right_f2.png");
    a->textures[TEX_HEAD_DOWN_F1]  = load_tex(r, "assets/sprites/player/snakeman_head_down_f1.png");
    a->textures[TEX_HEAD_DOWN_F2]  = load_tex(r, "assets/sprites/player/snakeman_head_down_f2.png");
    a->textures[TEX_HEAD_LEFT_F1]  = load_tex(r, "assets/sprites/player/snakeman_head_left_f1.png");
    a->textures[TEX_HEAD_LEFT_F2]  = load_tex(r, "assets/sprites/player/snakeman_head_left_f2.png");

    a->textures[TEX_BODY_H]       = load_tex(r, "assets/sprites/player/snakeman_body_straight_h.png");
    a->textures[TEX_BODY_V]       = load_tex(r, "assets/sprites/player/snakeman_body_straight_v.png");
    a->textures[TEX_BODY_TURN_UR] = load_tex(r, "assets/sprites/player/snakeman_body_turn_ur.png");
    a->textures[TEX_BODY_TURN_RD] = load_tex(r, "assets/sprites/player/snakeman_body_turn_rd.png");
    a->textures[TEX_BODY_TURN_DL] = load_tex(r, "assets/sprites/player/snakeman_body_turn_dl.png");
    a->textures[TEX_BODY_TURN_LU] = load_tex(r, "assets/sprites/player/snakeman_body_turn_lu.png");

    a->textures[TEX_TAIL_UP_F1]    = load_tex(r, "assets/sprites/player/snakeman_tail_up_f1.png");
    a->textures[TEX_TAIL_UP_F2]    = load_tex(r, "assets/sprites/player/snakeman_tail_up_f2.png");
    a->textures[TEX_TAIL_RIGHT_F1] = load_tex(r, "assets/sprites/player/snakeman_tail_right_f1.png");
    a->textures[TEX_TAIL_RIGHT_F2] = load_tex(r, "assets/sprites/player/snakeman_tail_right_f2.png");
    a->textures[TEX_TAIL_DOWN_F1]  = load_tex(r, "assets/sprites/player/snakeman_tail_down_f1.png");
    a->textures[TEX_TAIL_DOWN_F2]  = load_tex(r, "assets/sprites/player/snakeman_tail_down_f2.png");
    a->textures[TEX_TAIL_LEFT_F1]  = load_tex(r, "assets/sprites/player/snakeman_tail_left_f1.png");
    a->textures[TEX_TAIL_LEFT_F2]  = load_tex(r, "assets/sprites/player/snakeman_tail_left_f2.png");

    a->textures[TEX_POWERUP_MAGNET_F1]     = load_tex(r, "assets/sprites/powerup_magnet_f1.png");
    a->textures[TEX_POWERUP_MAGNET_F2]     = load_tex(r, "assets/sprites/powerup_magnet_f2.png");
    a->textures[TEX_POWERUP_EMP_F1]        = load_tex(r, "assets/sprites/powerup_emp_f1.png");
    a->textures[TEX_POWERUP_EMP_F2]        = load_tex(r, "assets/sprites/powerup_emp_f2.png");
    a->textures[TEX_POWERUP_SHIELD_F1]     = load_tex(r, "assets/sprites/powerup_shield_f1.png");
    a->textures[TEX_POWERUP_SHIELD_F2]     = load_tex(r, "assets/sprites/powerup_shield_f2.png");
    a->textures[TEX_POWERUP_OVERCLOCK_F1]  = load_tex(r, "assets/sprites/powerup_overclock_f1.png");
    a->textures[TEX_POWERUP_OVERCLOCK_F2]  = load_tex(r, "assets/sprites/powerup_overclock_f2.png");
    a->textures[TEX_POWERUP_MULTIPLIER_F1] = load_tex(r, "assets/sprites/powerup_multiplier_orb_f1.png");
    a->textures[TEX_POWERUP_MULTIPLIER_F2] = load_tex(r, "assets/sprites/powerup_multiplier_orb_f2.png");

    a->textures[TEX_UI_TITLE]      = load_tex(r, "assets/ui/title_backdrop.png");
    a->textures[TEX_UI_PAUSED]     = load_tex(r, "assets/ui/paused_overlay_frame.png");
    a->textures[TEX_UI_GAME_OVER]  = load_tex(r, "assets/ui/game_over_panel.png");
    a->textures[TEX_UI_HIGHSCORES] = load_tex(r, "assets/ui/highscore_panel.png");
    a->textures[TEX_UI_NEW_HIGH]   = load_tex(r, "assets/ui/new_high_score_badge.png");

    a->textures[TEX_FX_COMBO] = load_tex(r, "assets/fx/icon_combo.png");
    a->textures[TEX_FX_RISK]  = load_tex(r, "assets/fx/icon_risk_bonus.png");
    a->textures[TEX_FX_HIT]   = load_tex(r, "assets/fx/fx_hit_flash.png");
}

static void free_assets(Assets *a) {
    int i;
    for (i = 0; i < TEX_COUNT; ++i) {
        if (a->textures[i]) {
            SDL_DestroyTexture(a->textures[i]);
            a->textures[i] = NULL;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Draw helpers                                                        */
/* ------------------------------------------------------------------ */

static void draw_tex(SDL_Renderer *r, SDL_Texture *tex, int x, int y, int w, int h) {
    if (tex) {
        SDL_Rect dst = {x, y, w, h};
        SDL_RenderCopy(r, tex, NULL, &dst);
    }
}

static void draw_tile(SDL_Renderer *r, SDL_Texture *tex, int gx, int gy) {
    draw_tex(r, tex, gx * TILE_SIZE, gy * TILE_SIZE, TILE_SIZE, TILE_SIZE);
}

/* ------------------------------------------------------------------ */
/* 5x7 bitmap font rendered with SDL draw primitives (no font needed)  */
/* ------------------------------------------------------------------ */

#define FONT_W 5
#define FONT_H 7
#define FONT_STRIDE (FONT_W + 1)   /* 1-pixel gap between characters */

static void draw_char(SDL_Renderer *r, char c, int x, int y, int scale) {
    /*
     * Each glyph is a 5-column x 7-row bitmap packed into the low 35 bits
     * of a uint64_t.  Bit 34 is top-left, bit 0 is bottom-right.
     *
     *   Row 0:  bits 34-30  (top)
     *   Row 1:  bits 29-25
     *   ...
     *   Row 6:  bits  4- 0  (bottom)
     */
    static const uint64_t glyphs[128] = {
        ['0'] = 0x3A33AE62EULL,  /* .###. / #...# / #..## / #.#.# / ##..# / #...# / .###. */
        ['1'] = 0x11842108EULL,  /* ..#.. / .##.. / ..#.. / ..#.. / ..#.. / ..#.. / .###. */
        ['2'] = 0x3A213221FULL,  /* .###. / #...# / ....# / ..##. / .#... / #.... / ##### */
        ['3'] = 0x3A213062EULL,  /* .###. / #...# / ....# / ..##. / ....# / #...# / .###. */
        ['4'] = 0x08CA97C42ULL,  /* ...#. / ..##. / .#.#. / #..#. / ##### / ...#. / ...#. */
        ['5'] = 0x7E1E0862EULL,  /* ##### / #.... / ####. / ....# / ....# / #...# / .###. */
        ['6'] = 0x3A10F462EULL,  /* .###. / #.... / #.... / ####. / #...# / #...# / .###. */
        ['7'] = 0x7C2222108ULL,  /* ##### / ....# / ...#. / ..#.. / .#... / .#... / .#... */
        ['8'] = 0x3A317462EULL,  /* .###. / #...# / #...# / .###. / #...# / #...# / .###. */
        ['9'] = 0x3A317844CULL,  /* .###. / #...# / #...# / .#### / ....# / ...#. / .##.. */

        ['A'] = 0x11518FE31ULL,  /* ..#.. / .#.#. / #...# / #...# / ##### / #...# / #...# */
        ['B'] = 0x7A31F463EULL,  /* ####. / #...# / #...# / ####. / #...# / #...# / ####. */
        ['C'] = 0x3A308422EULL,  /* .###. / #...# / #.... / #.... / #.... / #...# / .###. */
        ['D'] = 0x7A318C63EULL,  /* ####. / #...# / #...# / #...# / #...# / #...# / ####. */
        ['E'] = 0x7E10E421FULL,  /* ##### / #.... / #.... / ###.. / #.... / #.... / ##### */
        ['F'] = 0x7E10E4210ULL,  /* ##### / #.... / #.... / ###.. / #.... / #.... / #.... */
        ['G'] = 0x3A30BC62FULL,  /* .###. / #...# / #.... / #.### / #...# / #...# / .#### */
        ['H'] = 0x4631FC631ULL,  /* #...# / #...# / #...# / ##### / #...# / #...# / #...# */
        ['I'] = 0x38842108EULL,  /* .###. / ..#.. / ..#.. / ..#.. / ..#.. / ..#.. / .###. */
        ['J'] = 0x1C4210A4CULL,  /* ..### / ...#. / ...#. / ...#. / ...#. / #..#. / .##.. */
        ['K'] = 0x4654C5251ULL,  /* #...# / #..#. / #.#.. / ##... / #.#.. / #..#. / #...# */
        ['L'] = 0x42108421FULL,  /* #.... / #.... / #.... / #.... / #.... / #.... / ##### */
        ['M'] = 0x4775AC631ULL,  /* #...# / ##.## / #.#.# / #.#.# / #...# / #...# / #...# */
        ['N'] = 0x4739ACE71ULL,  /* #...# / ##..# / ##..# / #.#.# / #..## / #..## / #...# */
        ['O'] = 0x3A318C62EULL,  /* .###. / #...# / #...# / #...# / #...# / #...# / .###. */
        ['P'] = 0x7A31F4210ULL,  /* ####. / #...# / #...# / ####. / #.... / #.... / #.... */
        ['Q'] = 0x3A318D64DULL,  /* .###. / #...# / #...# / #...# / #.#.# / #..#. / .##.# */
        ['R'] = 0x7A31F5251ULL,  /* ####. / #...# / #...# / ####. / #.#.. / #..#. / #...# */
        ['S'] = 0x3A307062EULL,  /* .###. / #...# / #.... / .###. / ....# / #...# / .###. */
        ['T'] = 0x7C8421084ULL,  /* ##### / ..#.. / ..#.. / ..#.. / ..#.. / ..#.. / ..#.. */
        ['U'] = 0x46318C62EULL,  /* #...# / #...# / #...# / #...# / #...# / #...# / .###. */
        ['V'] = 0x463152884ULL,  /* #...# / #...# / #...# / .#.#. / .#.#. / ..#.. / ..#.. */
        ['W'] = 0x4631AD6AAULL,  /* #...# / #...# / #...# / #.#.# / #.#.# / #.#.# / .#.#. */
        ['X'] = 0x454421151ULL,  /* #...# / .#.#. / ..#.. / ..#.. / ..#.. / .#.#. / #...# */
        ['Y'] = 0x454421084ULL,  /* #...# / .#.#. / ..#.. / ..#.. / ..#.. / ..#.. / ..#.. */
        ['Z'] = 0x7C222221FULL,  /* ##### / ....# / ...#. / ..#.. / .#... / #.... / ##### */

        [' '] = 0x000000000ULL,
        ['.'] = 0x000000080ULL,  /* center dot on row 5 */
        [':'] = 0x008000080ULL,  /* dots at row 1 and row 5 */
        ['!'] = 0x108421004ULL,  /* ..#.. rows 0-4, blank row 5, ..#.. row 6 */
        ['-'] = 0x0000F8000ULL,  /* middle row = ##### */
        ['+'] = 0x0084F9080ULL,  /* ..#.. / ..#.. / ##### / ..#.. / ..#.. */
        ['#'] = 0x295F57D4AULL,
        ['x'] = 0x001151151ULL,  /* lowercase x: only bottom 5 rows */
        ['%'] = 0x674421173ULL,  /* ##..# / ##.#. / ..#.. / ..#.. / .#.## / #..## */
        ['<'] = 0x088882082ULL,
        ['>'] = 0x208208888ULL,
        ['/'] = 0x044222110ULL,  /* ....# / ...#. / ...#. / ..#.. / .#... / .#... / #.... */
        ['('] = 0x111084104ULL,  /* ..#.. / .#... / #.... / #.... / #.... / .#... / ..#.. */
        [')'] = 0x104108444ULL,  /* ..#.. / ...#. / ....# / ....# / ....# / ...#. / ..#.. */
        ['_'] = 0x00000001FULL,  /* bottom row all on */
    };
    uint64_t g;
    int row, col;
    unsigned char uc = (unsigned char)c;

    if (uc >= 128) return;
    g = glyphs[uc];
    if (g == 0 && c != ' ') return;

    for (row = 0; row < FONT_H; ++row) {
        for (col = 0; col < FONT_W; ++col) {
            int bit = (FONT_H - 1 - row) * FONT_W + (FONT_W - 1 - col);
            if (g & (1ULL << bit)) {
                SDL_Rect px = {x + col * scale, y + row * scale, scale, scale};
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}

static int string_width(const char *s, int scale) {
    int len = 0;
    while (s[len]) len++;
    return len * FONT_STRIDE * scale;
}

static void draw_string(SDL_Renderer *r, const char *s, int x, int y, int scale) {
    while (*s) {
        draw_char(r, *s, x, y, scale);
        x += FONT_STRIDE * scale;
        s++;
    }
}

/* Draw string horizontally centered on the screen (1280px wide) */
static void draw_string_centered(SDL_Renderer *r, const char *s, int y, int scale) {
    int w = string_width(s, scale);
    int x = (1280 - w) / 2;
    draw_string(r, s, x, y, scale);
}

/* Formatted number to string */
static void uint_to_str(char *buf, int bufsize, uint32_t val) {
    int i = 0;
    char tmp[16];
    if (val == 0) { tmp[i++] = '0'; }
    while (val > 0 && i < 15) {
        tmp[i++] = '0' + (char)(val % 10);
        val /= 10;
    }
    {
        int j = 0;
        while (i > 0 && j < bufsize - 1) {
            buf[j++] = tmp[--i];
        }
        buf[j] = '\0';
    }
}

/* ------------------------------------------------------------------ */
/* Rendering functions per state                                       */
/* ------------------------------------------------------------------ */

static void render_maze(SDL_Renderer *r, const Assets *a, const Maze *maze) {
    int row, col;
    for (row = 0; row < MAZE_ROWS; ++row) {
        for (col = 0; col < MAZE_COLS; ++col) {
            SDL_Texture *tex = (maze->tiles[row][col] == TILE_WALL)
                ? a->textures[TEX_TILE_WALL]
                : a->textures[TEX_TILE_FLOOR];
            draw_tile(r, tex, col, row);
        }
    }
}

static int get_head_tex(const Assets *a, Direction dir, int frame, SDL_Texture **out) {
    int base;
    (void)a;
    switch (dir) {
        case DIR_UP:    base = TEX_HEAD_UP_F1; break;
        case DIR_RIGHT: base = TEX_HEAD_RIGHT_F1; break;
        case DIR_DOWN:  base = TEX_HEAD_DOWN_F1; break;
        case DIR_LEFT:  base = TEX_HEAD_LEFT_F1; break;
        default:        base = TEX_HEAD_RIGHT_F1; break;
    }
    *out = a->textures[base + (frame & 1)];
    return 1;
}

static int get_tail_tex(const Assets *a, Direction dir, int frame, SDL_Texture **out) {
    int base;
    (void)a;
    switch (dir) {
        case DIR_UP:    base = TEX_TAIL_UP_F1; break;
        case DIR_RIGHT: base = TEX_TAIL_RIGHT_F1; break;
        case DIR_DOWN:  base = TEX_TAIL_DOWN_F1; break;
        case DIR_LEFT:  base = TEX_TAIL_LEFT_F1; break;
        default:        base = TEX_TAIL_RIGHT_F1; break;
    }
    *out = a->textures[base + (frame & 1)];
    return 1;
}

static SDL_Texture *get_body_tex(const Assets *a, Direction from, Direction to) {
    /* from = direction entering the segment, to = direction leaving */
    if (from == to) {
        /* Straight */
        if (from == DIR_LEFT || from == DIR_RIGHT)
            return a->textures[TEX_BODY_H];
        return a->textures[TEX_BODY_V];
    }
    /* Turns: determine which corner piece */
    if ((from == DIR_UP && to == DIR_RIGHT) || (from == DIR_LEFT && to == DIR_DOWN))
        return a->textures[TEX_BODY_TURN_UR];
    if ((from == DIR_RIGHT && to == DIR_DOWN) || (from == DIR_UP && to == DIR_LEFT))
        return a->textures[TEX_BODY_TURN_RD];
    if ((from == DIR_DOWN && to == DIR_LEFT) || (from == DIR_RIGHT && to == DIR_UP))
        return a->textures[TEX_BODY_TURN_DL];
    if ((from == DIR_LEFT && to == DIR_UP) || (from == DIR_DOWN && to == DIR_RIGHT))
        return a->textures[TEX_BODY_TURN_LU];

    /* Fallback */
    return a->textures[TEX_BODY_H];
}

static void render_player(SDL_Renderer *r, const Assets *a, const Player *p, int anim_frame) {
    int i;
    if (!p->alive && p->length == 0) return;

    /* Head */
    {
        SDL_Texture *tex = NULL;
        get_head_tex(a, p->dir, anim_frame, &tex);
        draw_tile(r, tex, p->segments[0].x, p->segments[0].y);
    }

    /* Body segments */
    for (i = 1; i < p->length - 1; ++i) {
        SDL_Texture *tex = get_body_tex(a, p->seg_dirs[i], p->seg_dirs[i - 1]);
        draw_tile(r, tex, p->segments[i].x, p->segments[i].y);
    }

    /* Tail */
    if (p->length > 1) {
        SDL_Texture *tex = NULL;
        int tail = p->length - 1;
        get_tail_tex(a, p->seg_dirs[tail], anim_frame, &tex);
        draw_tile(r, tex, p->segments[tail].x, p->segments[tail].y);
    }
}

static void render_robots(SDL_Renderer *r, const Assets *a, const Robot *robots, int anim_frame) {
    int i;
    int tex_idx = (anim_frame & 1) ? TEX_ROBOT_F2 : TEX_ROBOT_F1;
    for (i = 0; i < NUM_ROBOTS; ++i) {
        if (!robots[i].active) continue;
        /* Robots are 48x48, center on tile */
        int px = robots[i].pos.x * TILE_SIZE - 8;
        int py = robots[i].pos.y * TILE_SIZE - 8;
        draw_tex(r, a->textures[tex_idx], px, py, 48, 48);
    }
}

static void render_pickups(SDL_Renderer *r, const Assets *a, const Pickup *pickups, int count, int anim_frame) {
    int i;
    int tex_idx = (anim_frame & 1) ? TEX_PICKUP_F2 : TEX_PICKUP_F1;
    for (i = 0; i < count; ++i) {
        if (!pickups[i].active) continue;
        draw_tile(r, a->textures[tex_idx], pickups[i].pos.x, pickups[i].pos.y);
    }
}

static int powerup_tex_index(PowerupType type, int frame) {
    switch (type) {
        case POWERUP_MAGNET:         return TEX_POWERUP_MAGNET_F1 + (frame & 1);
        case POWERUP_EMP:            return TEX_POWERUP_EMP_F1 + (frame & 1);
        case POWERUP_SHIELD:         return TEX_POWERUP_SHIELD_F1 + (frame & 1);
        case POWERUP_OVERCLOCK:      return TEX_POWERUP_OVERCLOCK_F1 + (frame & 1);
        case POWERUP_MULTIPLIER_ORB: return TEX_POWERUP_MULTIPLIER_F1 + (frame & 1);
        default:                     return TEX_POWERUP_MAGNET_F1;
    }
}

static void render_powerups(SDL_Renderer *r, const Assets *a, const Powerup *powerups, int count, int anim_frame) {
    int i;
    for (i = 0; i < count; ++i) {
        if (!powerups[i].active) continue;
        int idx = powerup_tex_index(powerups[i].type, anim_frame);
        draw_tile(r, a->textures[idx], powerups[i].pos.x, powerups[i].pos.y);
    }
}

static void render_hud(SDL_Renderer *r, const Game *game) {
    char buf[64];
    int y = MAZE_ROWS * TILE_SIZE + 2;

    /* Dark HUD bar */
    {
        SDL_Rect bar = {0, MAZE_ROWS * TILE_SIZE, MAZE_COLS * TILE_SIZE, 720 - MAZE_ROWS * TILE_SIZE};
        SDL_SetRenderDrawColor(r, 10, 10, 20, 255);
        SDL_RenderFillRect(r, &bar);
    }

    /* Score */
    SDL_SetRenderDrawColor(r, 0, 255, 200, 255);
    draw_string(r, "SCORE", 8, y, 2);
    uint_to_str(buf, sizeof(buf), game->score.score);
    draw_string(r, buf, 76, y, 2);

    /* Combo */
    SDL_SetRenderDrawColor(r, 255, 200, 0, 255);
    {
        int combo_int = (int)(game->score.combo_multiplier * 10.0f + 0.5f);
        snprintf(buf, sizeof(buf), "x%d.%d", combo_int / 10, combo_int % 10);
        draw_string(r, buf, 220, y, 2);
    }

    /* Pickups */
    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    {
        int remaining = 0;
        int i;
        for (i = 0; i < game->world.pickup_count; ++i) {
            if (game->world.pickups[i].active) remaining++;
        }
        snprintf(buf, sizeof(buf), "DOTS %d", remaining);
        draw_string(r, buf, 380, y, 2);
    }

    /* Time */
    SDL_SetRenderDrawColor(r, 150, 150, 255, 255);
    snprintf(buf, sizeof(buf), "TIME %d", (int)game->run_time_sec);
    draw_string(r, buf, 580, y, 2);

    /* Active powerup indicator */
    if (game->world.active_powerup.type != POWERUP_NONE) {
        const char *pname = "POWER";
        switch (game->world.active_powerup.type) {
            case POWERUP_MAGNET: pname = "MAGNET"; break;
            case POWERUP_EMP: pname = "EMP"; break;
            case POWERUP_SHIELD: pname = "SHIELD"; break;
            case POWERUP_OVERCLOCK: pname = "TURBO"; break;
            case POWERUP_MULTIPLIER_ORB: pname = "MULTI"; break;
            default: break;
        }
        SDL_SetRenderDrawColor(r, 255, 100, 255, 255);
        draw_string(r, pname, 790, y, 2);
    }

    /* Length */
    SDL_SetRenderDrawColor(r, 100, 255, 100, 255);
    snprintf(buf, sizeof(buf), "LEN %d", game->world.player.length);
    draw_string(r, buf, 990, y, 2);
}

static void render_running(SDL_Renderer *r, const Assets *a, const Game *game, int anim_frame) {
    render_maze(r, a, &game->world.maze);
    render_pickups(r, a, game->world.pickups, game->world.pickup_count, anim_frame);
    render_powerups(r, a, game->world.powerups, game->world.powerup_count, anim_frame);
    render_player(r, a, &game->world.player, anim_frame);
    render_robots(r, a, game->world.robots, anim_frame);
    render_hud(r, game);
}

/* ------------------------------------------------------------------ */
/* Random snake facts for the title screen                             */
/* ------------------------------------------------------------------ */

static const char *snake_facts[] = {
    "SNAKES ARE FOUND ON EVERY CONTINENT EXCEPT ANTARCTICA",
    "THE RETICULATED PYTHON CAN GROW UP TO 6.95 METERS LONG",
    "MOST SNAKES HAVE ONLY ONE FUNCTIONAL LUNG",
    "SNAKES SMELL WITH THEIR TONGUES USING A JACOBSON ORGAN",
    "SOME SNAKES CAN GO UP TO TWO YEARS WITHOUT EATING",
    "THE KING COBRA CAN GROW UP TO 5.85 METERS LONG",
    "THERE ARE MORE THAN 4100 KNOWN SPECIES OF SNAKES",
    "SNAKES EVOLVED FROM LIZARDS OVER 100 MILLION YEARS AGO",
    "SEA SNAKES CAN HOLD THEIR BREATH FOR UP TO 2 HOURS",
    "THE SMALLEST SNAKE IS JUST 10 CM LONG",
    "SNAKES SHED THEIR SKIN UP TO 12 TIMES PER YEAR",
    "FLYING SNAKES CAN GLIDE UP TO 100 METERS THROUGH THE AIR",
};

#define NUM_SNAKE_FACTS (int)(sizeof(snake_facts) / sizeof(snake_facts[0]))

static int current_fact_index = 0;

static void pick_random_fact(void) {
    current_fact_index = rand() % NUM_SNAKE_FACTS;
}

static void render_title(SDL_Renderer *r, const Assets *a) {
    /* Full-screen backdrop */
    if (a->textures[TEX_UI_TITLE]) {
        SDL_RenderCopy(r, a->textures[TEX_UI_TITLE], NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(r, 20, 28, 44, 255);
        SDL_RenderClear(r);
    }

    /* Title text overlay */
    SDL_SetRenderDrawColor(r, 0, 255, 220, 255);
    draw_string_centered(r, "SNAKEMAN", 160, 5);

    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    draw_string_centered(r, "PRESS ENTER TO START", 310, 3);
    draw_string_centered(r, "Q TO QUIT", 380, 2);

    SDL_SetRenderDrawColor(r, 100, 100, 120, 255);
    draw_string_centered(r, "WASD OR ARROWS TO MOVE", 460, 2);
    draw_string_centered(r, "P TO PAUSE  R TO RESTART", 490, 2);

    /* Random snake fact */
    SDL_SetRenderDrawColor(r, 180, 160, 80, 255);
    draw_string_centered(r, "DID YOU KNOW...", 580, 2);
    draw_string_centered(r, snake_facts[current_fact_index], 610, 2);
}

static void render_paused(SDL_Renderer *r, const Assets *a, const Game *game, int anim_frame) {
    /* Render game behind */
    render_running(r, a, game, anim_frame);

    /* Darken overlay */
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 160);
    {
        SDL_Rect full = {0, 0, 1280, 720};
        SDL_RenderFillRect(r, &full);
    }

    /* Paused panel */
    if (a->textures[TEX_UI_PAUSED]) {
        SDL_RenderCopy(r, a->textures[TEX_UI_PAUSED], NULL, NULL);
    }

    SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
    draw_string_centered(r, "PAUSED", 280, 4);

    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    draw_string_centered(r, "P TO RESUME", 370, 3);
    draw_string_centered(r, "Q TO QUIT", 440, 2);
}

static void render_game_over(SDL_Renderer *r, const Assets *a, const Game *game) {
    char buf[64];

    if (a->textures[TEX_UI_GAME_OVER]) {
        SDL_RenderCopy(r, a->textures[TEX_UI_GAME_OVER], NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(r, 40, 10, 10, 255);
        SDL_RenderClear(r);
    }

    SDL_SetRenderDrawColor(r, 255, 60, 60, 255);
    draw_string_centered(r, "GAME OVER", 140, 4);

    /* Score as combined label + value, centered */
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    {
        char score_line[80];
        uint_to_str(buf, sizeof(buf), game->score.score);
        snprintf(score_line, sizeof(score_line), "SCORE  %s", buf);
        draw_string_centered(r, score_line, 260, 3);
    }

    /* Stats block — all centered */
    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    snprintf(buf, sizeof(buf), "PICKUPS  %d", (int)game->score.pickups);
    draw_string_centered(r, buf, 340, 2);

    {
        int combo_int = (int)(game->score.max_combo_multiplier * 10.0f + 0.5f);
        snprintf(buf, sizeof(buf), "MAX COMBO  x%d.%d", combo_int / 10, combo_int % 10);
        draw_string_centered(r, buf, 370, 2);
    }

    {
        uint32_t t = game->run_time_sec;
        snprintf(buf, sizeof(buf), "TIME  %d:%02d", (int)(t / 60), (int)(t % 60));
        draw_string_centered(r, buf, 400, 2);
    }

    SDL_SetRenderDrawColor(r, 150, 150, 150, 255);
    draw_string_centered(r, "PRESS ENTER TO CONTINUE", 520, 2);
}

static void render_highscores(SDL_Renderer *r, const Assets *a, const Game *game) {
    char buf[64];
    int i;
    int y_start;
    int row_h = 46;
    /* Font heights: scale 3 -> 7*3 = 21px, scale 2 -> 7*2 = 14px */
    int font_h_lg = FONT_H * 3;
    int font_h_sm = FONT_H * 2;

    /* Dark background — ignore baked-in panel PNG to avoid misaligned bars */
    (void)a;
    SDL_SetRenderDrawColor(r, 18, 14, 28, 255);
    SDL_RenderClear(r);

    SDL_SetRenderDrawColor(r, 200, 100, 255, 255);
    draw_string_centered(r, "HIGH SCORES", 30, 3);

    /* New high score badge */
    if (game->last_run_rank == 0 && a->textures[TEX_UI_NEW_HIGH]) {
        draw_tex(r, a->textures[TEX_UI_NEW_HIGH], (1280 - 512) / 2, 65, 512, 48);
    } else if (game->last_run_rank == 0) {
        SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
        draw_string_centered(r, "NEW HIGH SCORE!", 75, 2);
    }

    /* Table layout: define column X positions relative to a left margin.
       Table is ~800px wide, centered: left = (1280 - 800) / 2 = 240 */
    {
        int tbl_x = 240;  /* table left edge */
        int tbl_w = 800;
        int col_rank  = tbl_x;
        int col_score = tbl_x + 130;
        int col_time  = tbl_x + 400;
        int col_dots  = tbl_x + 570;
        int col_arrow = tbl_x + 710;

        /* Column headers */
        y_start = 120;
        SDL_SetRenderDrawColor(r, 160, 140, 180, 255);
        draw_string(r, "RANK", col_rank + 10, y_start, 2);
        draw_string(r, "SCORE", col_score, y_start, 2);
        draw_string(r, "TIME", col_time, y_start, 2);
        draw_string(r, "DOTS", col_dots, y_start, 2);

        /* Thin separator line under headers */
        SDL_SetRenderDrawColor(r, 80, 60, 100, 255);
        {
            SDL_Rect sep = {tbl_x, y_start + 18, tbl_w, 1};
            SDL_RenderFillRect(r, &sep);
        }

        y_start += 26;
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

        for (i = 0; i < 10; ++i) {
            int y = y_start + i * row_h;
            int is_current = (i == game->last_run_rank);
            int has_score = (game->highscores.entries[i].score > 0);
            int text_y = y + (row_h - font_h_lg) / 2;  /* vertically centered */
            int text_y_sm = y + (row_h - font_h_sm) / 2;  /* centered for small font */

            /* Row background bar */
            {
                SDL_Rect row_bg = {tbl_x - 4, y + 2, tbl_w + 8, row_h - 4};
                if (is_current) {
                    /* Highlighted current-run row */
                    SDL_SetRenderDrawColor(r, 0, 255, 200, 50);
                    SDL_RenderFillRect(r, &row_bg);
                    /* Border for current row */
                    SDL_SetRenderDrawColor(r, 0, 255, 200, 120);
                    SDL_RenderDrawRect(r, &row_bg);
                } else if (has_score) {
                    /* Populated row — subtle dark bar */
                    SDL_SetRenderDrawColor(r, 40, 35, 60, 180);
                    SDL_RenderFillRect(r, &row_bg);
                } else {
                    /* Empty row — very dim */
                    SDL_SetRenderDrawColor(r, 30, 25, 45, 120);
                    SDL_RenderFillRect(r, &row_bg);
                }
            }

            /* Text color */
            if (is_current) {
                SDL_SetRenderDrawColor(r, 0, 255, 200, 255);
            } else if (has_score) {
                SDL_SetRenderDrawColor(r, 210, 210, 220, 255);
            } else {
                SDL_SetRenderDrawColor(r, 70, 65, 90, 255);
            }

            /* Rank number */
            if (i < 9) {
                snprintf(buf, sizeof(buf), " %d.", i + 1);
            } else {
                snprintf(buf, sizeof(buf), "%d.", i + 1);
            }
            draw_string(r, buf, col_rank, text_y, 3);

            if (has_score) {
                /* Score */
                uint_to_str(buf, sizeof(buf), game->highscores.entries[i].score);
                draw_string(r, buf, col_score, text_y, 3);

                /* Time (smaller font, vertically centered) */
                {
                    uint32_t t = game->highscores.entries[i].time_sec;
                    snprintf(buf, sizeof(buf), "%d:%02d", (int)(t / 60), (int)(t % 60));
                }
                if (!is_current) {
                    SDL_SetRenderDrawColor(r, 150, 150, 170, 255);
                }
                draw_string(r, buf, col_time, text_y_sm, 2);

                /* Pickups */
                snprintf(buf, sizeof(buf), "%d", (int)game->highscores.entries[i].pickups);
                draw_string(r, buf, col_dots, text_y_sm, 2);

                /* Pointer for current run */
                if (is_current) {
                    SDL_SetRenderDrawColor(r, 0, 255, 200, 255);
                    draw_string(r, "<", col_arrow, text_y, 3);
                }
            } else {
                draw_string(r, "---", col_score, text_y, 3);
            }
        }
    }

    /* Instructions */
    {
        int bot_y = y_start + 10 * row_h + 12;
        if (bot_y > 670) bot_y = 670;
        SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
        draw_string_centered(r, "ENTER RESTART   Q QUIT", bot_y, 2);
    }
}

/* ------------------------------------------------------------------ */
/* Input handling                                                      */
/* ------------------------------------------------------------------ */

static InputState input_from_frame(int *should_exit) {
    SDL_Event event;
    InputState input;

    input.direction = INPUT_DIR_NONE;
    input.start_pressed = 0;
    input.pause_pressed = 0;
    input.resume_pressed = 0;
    input.restart_pressed = 0;
    input.quit_pressed = 0;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            input.quit_pressed = 1;
            *should_exit = 1;
        }

        if (event.type != SDL_KEYDOWN) {
            continue;
        }

        switch (event.key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                input.direction = INPUT_DIR_UP;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                input.direction = INPUT_DIR_DOWN;
                break;
            case SDLK_LEFT:
            case SDLK_a:
                input.direction = INPUT_DIR_LEFT;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                input.direction = INPUT_DIR_RIGHT;
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                input.start_pressed = 1;
                break;
            case SDLK_r:
                input.restart_pressed = 1;
                break;
            case SDLK_p:
                input.pause_pressed = 1;
                input.resume_pressed = 1;
                break;
            case SDLK_t:
                /* Could add back-to-title from pause */
                break;
            case SDLK_q:
            case SDLK_ESCAPE:
                input.quit_pressed = 1;
                break;
            default:
                break;
        }
    }

    return input;
}

/* ------------------------------------------------------------------ */
/* Main app loop                                                       */
/* ------------------------------------------------------------------ */

int sdl_run_app(const GameConfig *config) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int should_exit;
    uint64_t perf_freq;
    uint64_t previous_counter;
    float accumulator;
    float fixed_dt;
    float anim_accum;
    int anim_frame;
    Game game;
    Assets assets;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        fprintf(stderr, "IMG_Init failed: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow(
        "Snakeman",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config->window_width,
        config->window_height,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    load_assets(renderer, &assets);
    game_init(&game, config);

    srand((unsigned)time(NULL));
    pick_random_fact();

    should_exit = 0;
    perf_freq = SDL_GetPerformanceFrequency();
    previous_counter = SDL_GetPerformanceCounter();
    accumulator = 0.0f;
    fixed_dt = 1.0f / (float)config->target_fps;
    anim_accum = 0.0f;
    anim_frame = 0;

    GameState prev_state = game.state;

    while (!should_exit && game.state != GAME_STATE_QUIT) {
        uint64_t current_counter = SDL_GetPerformanceCounter();
        uint64_t delta_counter = current_counter - previous_counter;
        float frame_dt = (float)delta_counter / (float)perf_freq;
        InputState input = input_from_frame(&should_exit);

        previous_counter = current_counter;

        /* Clamp frame_dt to avoid spiral of death */
        if (frame_dt > 0.25f) frame_dt = 0.25f;

        accumulator += frame_dt;

        while (accumulator >= fixed_dt) {
            game_update(&game, &input, fixed_dt);
            accumulator -= fixed_dt;
        }

        /* Pick a new snake fact each time we enter the title screen */
        if (game.state == GAME_STATE_TITLE && prev_state != GAME_STATE_TITLE) {
            pick_random_fact();
        }
        prev_state = game.state;

        /* Animation timer (~10 FPS) */
        anim_accum += frame_dt;
        if (anim_accum >= 0.1f) {
            anim_accum -= 0.1f;
            anim_frame++;
        }

        /* Render */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        switch (game.state) {
            case GAME_STATE_TITLE:
                render_title(renderer, &assets);
                break;
            case GAME_STATE_RUNNING:
                render_running(renderer, &assets, &game, anim_frame);
                break;
            case GAME_STATE_PAUSED:
                render_paused(renderer, &assets, &game, anim_frame);
                break;
            case GAME_STATE_GAME_OVER:
                render_game_over(renderer, &assets, &game);
                break;
            case GAME_STATE_HIGHSCORES:
                render_highscores(renderer, &assets, &game);
                break;
            default:
                break;
        }

        SDL_RenderPresent(renderer);
    }

    free_assets(&assets);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
