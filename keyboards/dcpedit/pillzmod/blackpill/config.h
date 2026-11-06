// Copyright 2023 Ming-Gih Lam (@dcpedit)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define AUDIO_INIT_DELAY
#define AUDIO_PIN A10
#define AUDIO_PWM_DRIVER PWMD1
#define AUDIO_PWM_CHANNEL 3
#define AUDIO_PWM_PAL_MODE 1
#define AUDIO_CLICKY
#define AUDIO_CLICKY_FREQ_RANDOMNESS 1.5f

#ifdef AUDIO_ENABLE
#define STARTUP_SONG SONG(STARTUP_SOUND)
#endif

#define LED_LAYER_PIN A2
#define LED_ACTIVE_LOW
/* Emulated EEPROM lives in flash sector 1 (16 KiB). Give the wear-leveling
 * log the whole sector instead of the default half, so the 4 KiB logical
 * EEPROM (10 layers of this 15x7 matrix plus Vial data) sees fewer erases. */
#define WEAR_LEVELING_BACKING_SIZE 16384
#define WEAR_LEVELING_LOGICAL_SIZE 4096
