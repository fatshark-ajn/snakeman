# Snakeman Game Design

This document consolidates the current game design decisions for Snakeman, including gameplay systems, scoring, state flow, and architecture choices for SDL2 now with future WASM support.

## Vision and Core Pillars

- Control a multi-segment creature that is always moving forward through a maze-like playfield.
- Consume pickups to grow and increase score.
- Avoid four hostile robots chasing the player.
- Balance route planning, survival pressure, and score optimization.
- Support a strong replay loop via highscores and fast restart.

## Core Gameplay Loop

1. Start a new run in a procedurally generated connected maze.
2. Navigate continuously, steering around walls and robots.
3. Collect pickups to grow by one segment and gain points.
4. Use powerups to create escape/scoring opportunities.
5. Manage increasing pressure as difficulty scales.
6. On death, show run breakdown and highscore ranking.
7. Let player restart immediately or quit.

## Playfield and Maze

### Grid and Layout

- The playfield is a tile grid covering the full game window.
- Each tile is wall or traversable space.
- All gameplay entities are represented on grid coordinates.

### Maze Generation

- v1 uses procedural generation per run.
- Generated layout must be connected and navigable.
- Spawn zones must guarantee valid initial placement for player, robots, and pickups.

## Player (Snakeman)

### Movement

- Constant forward movement driven by fixed-step simulation.
- Player input steers direction (`WASD` or arrow keys).
- Direction changes are constrained by wall collision rules.

### Growth

- Each consumed pickup adds one growth unit.
- Growth is realized by delaying tail advancement for one step.

### Collision

- Robot collision causes immediate death in baseline rules.
- Shield powerup can consume one collision instead of ending run.
- Self-collision can be deferred or enabled as a harder mode.

## Hostile Robots

- Four robots are active simultaneously.
- v1 AI uses simple steering chase behavior.
- Robots bias movement toward reducing distance to player head while respecting walls.
- Fallback logic avoids deadlock and repeated reversals.

## Pickups and Powerups

### Standard Pickups

- Primary objective items placed throughout reachable maze cells.
- Every pickup increases score and player length.
- Run completion is tied to consuming all required pickups.

### Powerups

1. Magnet (`8s`)
   - Attracts nearby pickups toward player route.
   - Magnet-assisted pickup grants `+5` bonus.
2. EMP Pulse (`5s`)
   - Slows robot movement by about `40%`.
   - Pickups during EMP gain `+20%` score.
3. Shield (`1 hit`)
   - First robot collision consumes shield instead of ending run.
   - Unused shield at level end grants `+100`.
4. Overclock (`6s`)
   - Increases player speed by about `20%`.
   - Reduces combo window to `2.5s` while active.
5. Multiplier Orb (`5s`)
   - Freezes current combo multiplier.
   - Temporarily extends multiplier cap by `+0.5`.

## Scoring System

### Base Scoring

- Pickup: `+10`.
- Survival tick: `+1` point per second alive.

### Combo System

- Pickup starts or refreshes combo timer (`3.5s` default).
- While active, each additional pickup adds `+0.25` multiplier.
- Multiplier cap is `x4.0`.
- Expired timer resets multiplier to `x1.0`.

### Situational Bonuses

- Maze control bonus: clear a quadrant without hit, `+75`.
- Risk bonus: pickup with robot within 4 tiles, `+50%` pickup score.

### Skill/Streak Awards

- Clean Path: every 20 clean tiles, `+30`.
- Chain Hunter: 5 pickups in one combo `+50`, 10 pickups `+150`.
- Close Call: escape immediate danger window, `+40`.
- Perfect Clear: no shield use and no robot-contact events, `+300`.

## Dynamic Difficulty

At each 25% pickup-clear milestone:

- Robot speed increases by `+5%`.
- Advanced powerup spawn chance increases slightly.
- Combo timer shrinks by `0.2s` to a floor of `2.2s`.

Fairness constraint:

- Robot speed cap is `player_speed * 0.92` in v1.

## Death, Results, and Highscore Flow

### On Death

1. Freeze simulation.
2. Show Game Over panel with:
   - final score,
   - pickups collected,
   - max combo,
   - survival time,
   - earned bonuses.
3. Transition to Highscore screen.

### Highscore Screen

- Display top 10 entries.
- Highlight current run rank.
- Show "NEW HIGH SCORE" indicator for rank 1.
- If outside top 10, show rank estimate.
- Controls: `R` restart, `Q` or `Esc` quit.

## Runtime State Machine

Primary states:

- `BOOT`
- `TITLE`
- `RUNNING`
- `PAUSED` (optional in v1)
- `GAME_OVER`
- `HIGHSCORES`
- `QUIT`

Core simulation runs only in `RUNNING`.

Transition outline:

- `BOOT -> TITLE`
- `TITLE -> RUNNING` or `QUIT`
- `RUNNING -> PAUSED`, `GAME_OVER`, optional direct win to `HIGHSCORES`
- `PAUSED -> RUNNING` or `TITLE`
- `GAME_OVER -> HIGHSCORES`
- `HIGHSCORES -> RUNNING`, `TITLE`, or `QUIT`

Transition priority per tick:

1. Death/fatal events
2. Run completion events
3. User overlay/menu actions

## Architecture for Native + Future WASM

### Core vs Platform Separation

- Core (platform-agnostic C11): gameplay rules, scoring, AI, state logic.
- Platform adapter: SDL input polling, render backend, timing, persistence.

### Input Abstraction

- Platform produces `InputState` each frame.
- Core consumes `InputState` only.
- No SDL event types in core modules.

### Rendering Abstraction

- Core emits render data/commands.
- Platform translates to actual drawing calls.
- This enables desktop and web backends without gameplay rewrites.

### Timing Abstraction

- Use fixed-step simulation in core for deterministic behavior.
- Platform provides frame delta for accumulator update loop.

### Persistence Abstraction

- Core owns scoring math and highscore insertion logic.
- Platform owns storage implementation.
- Desktop target: local file.
- Web target: `localStorage`.

## Data Model Draft

```c
typedef struct {
    uint32_t score;
    uint32_t time_sec;
    uint16_t pickups;
    uint8_t max_combo_x100;
    uint32_t seed;
} ScoreEntry;

typedef struct {
    ScoreEntry entries[10];
} HighscoreTable;
```

Suggested core APIs:

- `game_init(Game*, const GameConfig*)`
- `game_update(Game*, const InputState*, float fixed_dt)`
- `game_build_render_list(const Game*, RenderList*)`
- `score_on_event(...)`
- `score_finalize_run(...)`
- `highscore_try_insert(...)`

## Controls (v1)

- Move: `WASD` or Arrow keys
- Pause: `P` (optional `Esc` in running state)
- Restart from end screens: `R` or Enter
- Quit: `Q` or `Esc`

## Scope Guidance

Recommended minimal playable v1 includes:

- procedural maze,
- constant forward movement,
- growth from pickups,
- 4 simple-steering robots,
- scoring + combo,
- game-over breakdown,
- highscore screen with restart/quit.

Potential post-v1 additions:

- robot personality variants,
- self-collision hard mode,
- richer UI and audiovisual feedback,
- online leaderboard support.
