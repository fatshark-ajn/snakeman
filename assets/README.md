# Snakeman Placeholder Art Pack

This folder contains PNG-only placeholder assets in an 80s neo-arcade retro style.

## Visual Direction

- Neon-forward palette: cyan/teal primary, hot red danger, amber highlights, deep navy shadows.
- High-contrast silhouette readability for fast top-down gameplay.
- Subtle CRT flavor via light scanline/noise treatment.

## Naming and Animation

- All assets use `snake_case`.
- Animated variants use `_f1` and `_f2` suffixes.
- Recommended playback:
  - Character/enemy/pickup frames: 8-12 FPS
  - Powerup pulse frames: 6-8 FPS

## Dimensions

- `32x32`: player segments, pickups, powerups, tiles, FX icons
- `48x48`: robot enemy
- `1280x720`: full-screen UI backdrops/panels
- `512x128`: new high score badge

## Folder Map

### `assets/sprites/player/`

Multi-segment player sprite set (4-direction top-down):

- `snakeman_head_{up,right,down,left}_f1.png`
- `snakeman_head_{up,right,down,left}_f2.png`
- `snakeman_body_straight_h.png`
- `snakeman_body_straight_v.png`
- `snakeman_body_turn_ur.png`
- `snakeman_body_turn_rd.png`
- `snakeman_body_turn_dl.png`
- `snakeman_body_turn_lu.png`
- `snakeman_tail_{up,right,down,left}_f1.png`
- `snakeman_tail_{up,right,down,left}_f2.png`

Use head + body + tail composition at runtime to build snake length and path shape.

### `assets/sprites/`

- `robot_enemy_f1.png`, `robot_enemy_f2.png`
- `pickup_dot_f1.png`, `pickup_dot_f2.png`
- `powerup_magnet_f1.png`, `powerup_magnet_f2.png`
- `powerup_emp_f1.png`, `powerup_emp_f2.png`
- `powerup_shield_f1.png`, `powerup_shield_f2.png`
- `powerup_overclock_f1.png`, `powerup_overclock_f2.png`
- `powerup_multiplier_orb_f1.png`, `powerup_multiplier_orb_f2.png`

### `assets/tiles/`

- `tile_floor.png`
- `tile_wall.png`
- `tile_hazard.png`
- `tile_quadrant_accent_a.png`
- `tile_quadrant_accent_b.png`

### `assets/ui/`

- `title_backdrop.png`
- `paused_overlay_frame.png`
- `game_over_panel.png`
- `highscore_panel.png`
- `new_high_score_badge.png`

### `assets/fx/`

- `icon_combo.png`
- `icon_risk_bonus.png`
- `icon_close_call.png`
- `icon_clean_path.png`
- `icon_perfect_clear.png`
- `fx_hit_flash.png`

## Integration Notes

- These are placeholders tuned for readability first, not final polish.
- Keep filenames stable for easy drop-in replacement with final art later.
- All sprite/icon assets are transparent PNGs; UI backdrops/panels are opaque.
