# Snakeman Game State Draft

This draft defines the runtime state machine for Snakeman, including transition rules, state responsibilities, and input handling. It is written to support native SDL2 now and WASM/web later.

## State Overview

- `BOOT`
- `TITLE`
- `RUNNING`
- `PAUSED` (optional in v1, recommended)
- `GAME_OVER`
- `HIGHSCORES`
- `QUIT`

Core game simulation should only run in `RUNNING`.

## Transition Map

```text
BOOT -> TITLE
TITLE -> RUNNING
TITLE -> QUIT
RUNNING -> PAUSED
PAUSED -> RUNNING
PAUSED -> TITLE
RUNNING -> GAME_OVER
RUNNING -> HIGHSCORES   (optional immediate win flow)
GAME_OVER -> HIGHSCORES
HIGHSCORES -> RUNNING   (restart/new run)
HIGHSCORES -> TITLE
HIGHSCORES -> QUIT
```

## State Contracts

Each state should define:

- Entry action (`on_enter`)
- Per-frame update (`on_update`)
- Render output (`on_render`)
- Exit action (`on_exit`)

Keep transitions explicit and centralized in a state manager.

## Detailed State Behavior

## BOOT

Purpose:

- Initialize platform systems (window, renderer, input, timer, storage).
- Load or create highscore table.
- Initialize default config and RNG seed source.

Transitions:

- On success: `BOOT -> TITLE`
- On fatal init error: `BOOT -> QUIT`

Notes:

- No gameplay simulation here.

## TITLE

Purpose:

- Display game title, control hints, and start prompt.
- Optionally show top 3 highscores and last run summary.

Input:

- `Start` (Enter/Space): begin run.
- `Quit` (Esc/Q): exit app.

Transitions:

- `TITLE -> RUNNING` on Start
- `TITLE -> QUIT` on Quit

Entry actions:

- Reset transient UI state.

## RUNNING

Purpose:

- Execute fixed-step core simulation.
- Process movement, collisions, scoring, powerups, and robot AI.

Input:

- Direction controls (`WASD`/arrows)
- `Pause` (P or Esc)

Transitions:

- `RUNNING -> PAUSED` on Pause input
- `RUNNING -> GAME_OVER` on player death
- Optional: `RUNNING -> HIGHSCORES` when run ends with a win condition and no game-over interstitial

Entry actions:

- New run init:
  - generate maze
  - place player/robots/pickups/powerups
  - reset score session and timers

Exit actions:

- If dying: finalize run summary packet for UI and highscore insertion check.

## PAUSED

Purpose:

- Freeze simulation while showing pause overlay.

Input:

- `Resume` (P/Enter)
- `Back to Title` (T)

Transitions:

- `PAUSED -> RUNNING` on Resume
- `PAUSED -> TITLE` on Back to Title

Notes:

- Timers should not advance while paused.

## GAME_OVER

Purpose:

- Show immediate death feedback and run breakdown.
- Emphasize cause of death and final score.

Recommended display duration:

- Minimum `1.0s` lock before input is accepted to avoid accidental skip.

Input:

- After lock: any confirm/start key to continue

Transitions:

- `GAME_OVER -> HIGHSCORES`

Entry actions:

- Freeze world snapshot.
- Build breakdown fields:
  - score
  - pickups
  - max combo
  - survival time
  - bonuses list

## HIGHSCORES

Purpose:

- Display top 10 entries and current run placement.
- Provide replay or exit decision.

Input:

- `Restart` (R/Enter): start new run.
- `Title` (T): return to title screen.
- `Quit` (Q/Esc): exit app.

Transitions:

- `HIGHSCORES -> RUNNING` on Restart
- `HIGHSCORES -> TITLE` on Title
- `HIGHSCORES -> QUIT` on Quit

Entry actions:

- Attempt highscore insert.
- Persist updated highscore table through platform storage adapter.

## QUIT

Purpose:

- Shutdown path and cleanup.

Entry actions:

- Persist pending settings/scores.
- Destroy renderer/window/input resources.
- Exit process.

## Event and Data Flow

- Platform layer gathers raw input each frame.
- App layer maps raw input into `InputState`.
- State manager decides transition based on current state + input + core events.
- Core emits game events (`pickup`, `powerup`, `death`, `win`), score events, and a render list.

Suggested event enum:

```c
typedef enum {
    GAME_EVENT_NONE = 0,
    GAME_EVENT_PICKUP_COLLECTED,
    GAME_EVENT_POWERUP_COLLECTED,
    GAME_EVENT_PLAYER_DIED,
    GAME_EVENT_ALL_PICKUPS_CLEARED
} GameEventType;
```

## Recommended Transition Priority (per update tick)

When multiple transition conditions happen in the same simulation tick:

1. Fatal (`PLAYER_DIED`)
2. Run completion (`ALL_PICKUPS_CLEARED`)
3. User pause/overlay actions

This avoids ambiguous outcomes and keeps logic deterministic.

## Persistence Responsibilities

- Core:
  - Produces run summary and highscore candidate entry.
  - Maintains sorted in-memory highscore table.
- Platform adapter:
  - Load/save serialized highscore data.
  - Desktop target: local file.
  - Web target: `localStorage`.

## WASM Compatibility Notes

- Keep state updates single-threaded and deterministic.
- Avoid blocking I/O in transitions.
- Treat `QUIT` on web as "return to title" or "show session ended" behavior where browser tab cannot truly be closed by app logic.

## Minimal v1 State Set

If reducing scope for first playable build:

- Keep: `TITLE`, `RUNNING`, `GAME_OVER`, `HIGHSCORES`, `QUIT`
- Defer: `PAUSED`

This still supports complete gameplay, clear loss handling, and replay loop.
