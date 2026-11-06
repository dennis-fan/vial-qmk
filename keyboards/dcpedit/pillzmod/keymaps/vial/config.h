// Copyright 2024 Ming-Gih Lam (@dcpedit)
/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

#define VIAL_KEYBOARD_UID {0x3D, 0x36, 0x94, 0xF9, 0x71, 0x35, 0x9D, 0x7E}

// Unlock combo: Esc (row 12, col 0) + left thumb Backspace (row 4, col 6)
#define VIAL_UNLOCK_COMBO_ROWS { 12, 4 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 6 }
// Hold the combo for 3 s (counter ticks every 100 ms) instead of the 5 s default
#define VIAL_UNLOCK_COUNTER_MAX 30

/* latest.vil was saved from a 10-layer build; keep the same layer count so the
 * file can still be loaded as-is from the Vial GUI. */
#undef DYNAMIC_KEYMAP_LAYER_COUNT
#define DYNAMIC_KEYMAP_LAYER_COUNT 10
#define VIAL_TAP_DANCE_ENTRIES 16
#define VIAL_COMBO_ENTRIES 16

/* QMK settings as saved in latest.vil. These are live Vial settings; the
 * values here are what a settings reset loads. */
#define TAPPING_TERM 150
#define VIAL_DEFAULT_QUICK_TAP_TERM 200
#define TAP_CODE_DELAY 0
#define VIAL_DEFAULT_CHORDAL_HOLD
#define VIAL_DEFAULT_FLOW_TAP_TERM 4
