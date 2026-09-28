/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define VIAL_KEYBOARD_UID {0x2F, 0xB8, 0x8E, 0x89, 0x9B, 0x0A, 0xF3, 0x39}

/* Backlight defaults (apply after an EEPROM reset; Vial keeps the live values) */
#undef RGB_MATRIX_DEFAULT_MODE
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_REACTIVE
#define RGB_MATRIX_DEFAULT_HUE 30 /* yellowish orange; 0 = red, 21 = orange, 43 = yellow */
#define RGB_MATRIX_DEFAULT_SAT 255

/* Timeless home row mods (urob / pgetreuer). Tapping term, permissive hold,
 * chordal hold and flow tap are live Vial settings; these are the values a
 * settings reset loads. Speculative hold is compile-time only. */
#define TAPPING_TERM 250
#define VIAL_DEFAULT_PERMISSIVE_HOLD
#define VIAL_DEFAULT_CHORDAL_HOLD
#define VIAL_DEFAULT_FLOW_TAP_TERM 150
#define SPECULATIVE_HOLD
