#pragma once

typedef enum {
    OFF = 0,
    STATIC,
    BREATHING,
    GRADIENT
} indicator_modes_t;

typedef enum {
    NOCHG = 0,
    CHARGING,
    FULL
} charging_status_t;

/* --- EEPROM Handlers --- */
void indicator_eeprom_init(void);

/* --- EFFECT: Gradient --- */
uint32_t gradient_callback(uint32_t trigger_time, void *cb_arg);

/* --- EFFECT: Breathing --- */
uint32_t breathing_callback(uint32_t trigger_time, void *cb_arg);

/* --- MODE SWITCHING AND PROCESSING --- */
void post_init_hooks(void);
void handle_indicator_oneshot_modes(void);
bool process_indicator_keycodes(uint16_t keycode, keyrecord_t *record);
void process_indicators(void);

/* --- STATUS: Battery --- */
void show_battery_indicator(void);

/* --- STATUS: Charging --- */
uint32_t charge_callback(uint32_t trigger_time, void *cb_arg);

/* --- UTILITES --- */
HSV get_indicator_hsv(void);
HSV hsv_blend(HSV c0, HSV c1, float t);
void change_indicator_mode(void);
void change_indicator_speed(int ds);
bool current_mode_requires_update(void);
bool overlay_active(void);
void update_indicator_color(int dh, int dv);
void set_indicator_hsv(HSV hsv);