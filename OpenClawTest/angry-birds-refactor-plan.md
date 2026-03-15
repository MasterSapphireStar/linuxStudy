# Angry Birds Demo Refactor Plan

## Goal
Turn the current single-file canvas demo into a genuinely playable mini-game while preserving:
- single HTML file
- no build system
- lightweight custom physics

## Core targets
1. Stable round resolution
2. Better aiming and launch cadence
3. Support-driven block falling and crush damage
4. Multi-level progression
5. Stronger in-game feedback and HUD
6. Cleaner in-file module boundaries

## Implementation order
1. Introduce data-driven levels
2. Split screenState / turnState
3. Rework launch / settle / next-bird lifecycle
4. Add simple falling-block support logic
5. Add chain damage to pigs from falling blocks
6. Upgrade HUD and status overlays
7. Improve visual feedback and background polish
8. Add next level / retry / game clear flows

## Non-goals for this pass
- external assets
- full rigid-body rotation physics
- mobile-perfect adaptation
- multiple bird species beyond minimal room for extension
