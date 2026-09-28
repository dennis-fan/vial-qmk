/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define VIAL_KEYBOARD_UID {0x2F, 0xB8, 0x8E, 0x89, 0x9B, 0x0A, 0xF3, 0x39}

/* Backlight defaults (apply after an EEPROM reset; Vial keeps the live values) */
#undef RGB_MATRIX_DEFAULT_MODE
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_REACTIVE
#define RGB_MATRIX_DEFAULT_HUE 30 /* yellowish orange; 0 = red, 21 = orange, 43 = yellow */
#define RGB_MATRIX_DEFAULT_SAT 255
