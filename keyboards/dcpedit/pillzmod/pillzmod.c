// Copyright 2024 Ming-Gih Lam (@dcpedit)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "quantum.h"

#define NUM_LAYER     1

void keyboard_pre_init_kb(void) {
    gpio_set_pin_output(LED_LAYER_PIN);
    keyboard_pre_init_user();
}

// Light the fourth status LED while the numpad layer is active
layer_state_t layer_state_set_kb(layer_state_t state) {
    bool on = IS_LAYER_ON_STATE(state, NUM_LAYER);
#ifdef LED_ACTIVE_LOW
    on = !on;
#endif
    gpio_write_pin(LED_LAYER_PIN, on);
    return layer_state_set_user(state);
}
