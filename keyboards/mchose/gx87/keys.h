#pragma once

#include QMK_KEYBOARD_H

enum my_keycodes {
    // Continue directly after the data-driven keycodes in keyboard.json (KC_USB..KC_2G4 = QK_KB_0..6)
    // so VIA/Vial customKeycodes, which map sequentially from QK_KB_0, line up with these.
    INDICATOR_HUEU = KC_2G4 + 1,
    INDICATOR_HUED,
    INDICATOR_SPDU,
    INDICATOR_SPDD,
    INDICATOR_VALU,
    INDICATOR_VALD,
    INDICATOR_MODE,
    INDICATOR_CHRG,
    BATTERY_LEVEL,

    ID_HUEU = INDICATOR_HUEU,
    ID_HUED = INDICATOR_HUED,
    ID_SPDU = INDICATOR_SPDU,
    ID_SPDD = INDICATOR_SPDD,
    ID_VALU = INDICATOR_VALU,
    ID_VALD = INDICATOR_VALD,
    ID_MODE = INDICATOR_MODE,
    ID_CHRG = INDICATOR_CHRG,
    BAT_LVL = BATTERY_LEVEL
};
