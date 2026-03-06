# Snakeman Scoring and Engagement Design

This document defines the in-run scoring loop, powerups, dynamic tension rules, and post-death highscore flow for Snakeman.

## Goals

- Reward skillful routing, risky play, and sustained momentum.
- Keep scoring readable during active play.
- Create a strong replay loop with highscores and immediate restart.
- Keep rules deterministic and platform-agnostic for native + future WASM builds.

## Core Scoring Rules

- Pickup: `+10` base points.
- Survival tick: `+1` point per second alive.

### Combo System

- Picking up an item starts or refreshes `combo_timer` (default `3.5s`).
- While timer is active, each additional pickup increases multiplier by `+0.25`.
- Multiplier cap is `x4.0`.
- If timer expires, multiplier resets to `x1.0`.

### Situational Bonuses

- Maze control bonus: clearing a full maze quadrant without being hit grants `+75`.
- Risk bonus: pickup collected while a robot is within `4` tiles receives `+50%` pickup score.

## Powerups

Powerups are temporary state modifiers intended to create tactical choices.

1. Magnet (`8s`)
   - Nearby pickups are attracted toward player path.
   - Magnet-assisted pickup grants `+5` extra points.
2. EMP Pulse (`5s`)
   - Robot movement speed reduced by roughly `40%`.
   - Pickups during EMP gain `+20%` score.
3. Shield (`1 hit`)
   - First robot collision consumes shield instead of ending run.
   - If shield remains unused at level end: `+100` bonus.
4. Overclock (`6s`)
   - Player speed increased by roughly `20%`.
   - Combo window reduced to `2.5s` while active (risk/reward tradeoff).
5. Multiplier Orb (`5s`)
   - Freezes current combo multiplier and extends cap by `+0.5` temporarily.

## Streak and Skill Awards

- Clean Path: every `20` tiles moved without close wall/collision mistakes gives `+30`.
- Chain Hunter:
  - `5` pickups in one combo: `+50`
  - `10` pickups in one combo: `+150`
- Close Call: robot distance shifts from `<=1` tile to `>=4` tiles within `1.5s`: `+40`.
- Perfect Clear: all pickups consumed with no shield usage and no robot-contact events: `+300`.

## Dynamic Difficulty

At each `25%` pickup-clear milestone:

- Robot speed increases by `+5%`.
- Advanced powerup spawn chance increases slightly.
- Combo timer shrinks by `0.2s` (floor `2.2s`).

Fairness guardrail:

- Robot speed is capped to `player_speed * 0.92` in v1.

## Death, Results, and Highscore Flow

When the player dies:

1. Freeze world simulation.
2. Show Game Over panel with run breakdown:
   - Final score
   - Pickups consumed
   - Max combo reached
   - Time survived
   - Bonuses earned
3. Transition to Highscore screen:
   - Top 10 entries
   - Current run highlighted with rank
   - "NEW HIGH SCORE" indicator for rank 1
   - Controls: `R` restart, `Q` or `Esc` quit

If run does not enter top 10, still display final rank estimate and run stats.

## Data Model

Keep score logic in core (platform-neutral C11).

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

Suggested core API:

- `score_on_event(...)`
- `score_finalize_run(...)`
- `highscore_try_insert(...)`

Persistence should be implemented in platform adapters:

- Desktop: local file (`scores.dat` or JSON)
- Web/WASM: browser `localStorage`

## Platform Abstraction Notes

- Do not couple scoring/highscore logic to SDL calls.
- Emit immutable run-summary data from core; render/UI adapters present it.
- Keep timers driven by fixed-step simulation for deterministic behavior across native and web targets.
