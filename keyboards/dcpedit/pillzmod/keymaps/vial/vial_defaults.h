// Generated from latest.vil by vil2keymap.py -- Vial EEPROM defaults.
#pragma once

#include "vial.h"

/* Macro buffer keycodes: every key used below is a basic (8-bit) keycode, which
 * Vial stores as a single byte after SS_DOWN_CODE/SS_UP_CODE/SS_TAP_CODE. */
#define VIAL_MACRO_KC(kc) ((uint8_t)(kc))

static const vial_tap_dance_entry_t PROGMEM vial_default_tap_dances[] = {
    /* TD(0) */ { KC_ENT, MO(2), KC_NO, KC_LSFT, 150 },
    /* TD(1) */ { KC_BSPC, MO(1), KC_NO, KC_NO, 150 },
    /* TD(2) */ { KC_SPC, MO(2), LSFT(KC_MINS), KC_SPC, 200 },
    /* TD(3) */ { KC_ENT, MO(3), KC_NO, KC_NO, 200 },
    /* TD(4) */ { KC_SPC, MO(4), KC_NO, KC_NO, 200 },
    /* TD(5) */ { LGUI(KC_TAB), KC_NO, KC_NO, KC_NO, 150 },
};

static const vial_combo_entry_t PROGMEM vial_default_combos[] = {
    /* combo 0 */ { { KC_C, KC_V, KC_NO, KC_NO }, MC_0 },
};

static const uint8_t PROGMEM vial_default_macro_buffer[] = {
    /* M0: down KC_LGUI KC_LSHIFT KC_A delay 50 up KC_LGUI KC_LSHIFT KC_A */
    0x01, 0x02, VIAL_MACRO_KC(KC_LGUI), 0x01, 0x02, VIAL_MACRO_KC(KC_LSFT), 0x01, 0x02, VIAL_MACRO_KC(KC_A), 0x01, 0x04, 0x33, 0x01, 0x01, 0x03, VIAL_MACRO_KC(KC_LGUI), 0x01, 0x03, VIAL_MACRO_KC(KC_LSFT), 0x01, 0x03, VIAL_MACRO_KC(KC_A), 0x00,
    /* M1: down KC_LALT KC_LGUI KC_LEFT delay 25 up KC_LALT KC_LGUI KC_LEFT */
    0x01, 0x02, VIAL_MACRO_KC(KC_LALT), 0x01, 0x02, VIAL_MACRO_KC(KC_LGUI), 0x01, 0x02, VIAL_MACRO_KC(KC_LEFT), 0x01, 0x04, 0x1a, 0x01, 0x01, 0x03, VIAL_MACRO_KC(KC_LALT), 0x01, 0x03, VIAL_MACRO_KC(KC_LGUI), 0x01, 0x03, VIAL_MACRO_KC(KC_LEFT), 0x00,
    /* M2: down KC_LALT KC_LGUI KC_RIGHT delay 25 up KC_LALT KC_LGUI KC_RIGHT */
    0x01, 0x02, VIAL_MACRO_KC(KC_LALT), 0x01, 0x02, VIAL_MACRO_KC(KC_LGUI), 0x01, 0x02, VIAL_MACRO_KC(KC_RGHT), 0x01, 0x04, 0x1a, 0x01, 0x01, 0x03, VIAL_MACRO_KC(KC_LALT), 0x01, 0x03, VIAL_MACRO_KC(KC_LGUI), 0x01, 0x03, VIAL_MACRO_KC(KC_RGHT), 0x00,
    /* M3: text dennisfan@gmail.com */
    0x64, 0x65, 0x6e, 0x6e, 0x69, 0x73, 0x66, 0x61, 0x6e, 0x40, 0x67, 0x6d, 0x61, 0x69, 0x6c, 0x2e, 0x63, 0x6f, 0x6d, 0x00,
    /* M4: text 4254695774 */
    0x34, 0x32, 0x35, 0x34, 0x36, 0x39, 0x35, 0x37, 0x37, 0x34, 0x00,
};
