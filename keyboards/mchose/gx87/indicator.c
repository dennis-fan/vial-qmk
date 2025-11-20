#ifdef RGB_MATRIX_ENABLE

#include "quantum.h"
#include "indicator.h"
#include "keys.h"
#include "module.h"

static bool battery_indicator_enabled = false;
static charging_status_t chg_status = NOCHG;
deferred_token dynamic_effect_token = INVALID_DEFERRED_TOKEN;
deferred_token charging_token = INVALID_DEFERRED_TOKEN;
static float gradient_offset = 0.0f;
static float breathe_offset = 1.0f;
static bool breathe_up = false;


/* --- EEPROM Handlers --- */
/* We use datablocks because WB Wireless also requires some EEPROM
 */

typedef union {
    uint32_t raw;
    struct {
        indicator_modes_t mode :3;
        uint16_t animation_speed :12;
        uint8_t hue :8;
        uint8_t val :8;
        bool charging_indicator_enabled : 1;
    };
} indicator_config_t;

indicator_config_t indicator_config;

void indicator_eeprom_init(void) {
    indicator_config.raw = 0;
    indicator_config.mode = GRADIENT;
    indicator_config.animation_speed = 256;
    indicator_config.hue = 0;
    indicator_config.val = 102;
    indicator_config.charging_indicator_enabled = true;

    eeconfig_update_kb_datablock(&indicator_config, WB_EECONFIG_DATA_SIZE, EECONFIG_KB_DATA_SIZE-WB_EECONFIG_DATA_SIZE);
}


/* --- EFFECT: Gradient --- */
/* Colors taken from OpenRGB Effect's Plugin
 * Color set: Vaporwave
 */
static const HSV palette[] = {
    {213, 142, 255},  // #ff71ce
    {188, 152, 255},  // #b967ff
    {135, 254, 254},  // #01cdfe
    {115, 250, 255},  // #05ffa1
    {39,  105, 255},  // #fffb96
    {213, 142, 255},  // #ff71ce (loop back)
};

uint32_t gradient_callback(uint32_t trigger_time, void *cb_arg) {
    if (overlay_active()) return indicator_config.animation_speed;

    const uint8_t n_colors = sizeof(palette)/sizeof(palette[0]) - 1;

    for (uint8_t i = 0; i < INDICATOR_LED_COUNT && !battery_indicator_enabled; i++) {
        float pos = i + gradient_offset;
        int idx0 = (int)pos % n_colors;
        int idx1 = (idx0 + 1) % n_colors;
        float t = pos - (int)pos;

        HSV blend = hsv_blend(palette[idx0], palette[idx1], t);

        blend.v = indicator_config.val; // don't blend brightness

        RGB rgb = hsv_to_rgb(blend);
        rgb_matrix_set_color(INDICATOR_LED_START + i, rgb.r, rgb.g, rgb.b);
    }

    gradient_offset += 0.05f;
    if (gradient_offset >= n_colors) gradient_offset -= n_colors;

    return indicator_config.animation_speed;
}


/* --- EFFECT: Breathing --- */
uint32_t breathing_callback(uint32_t trigger_time, void *cb_arg) {
    if (overlay_active()) return indicator_config.animation_speed/BREATHING_SPEEDUP_FACTOR;

    HSV breathe = get_indicator_hsv();
    breathe.v = (uint8_t)(indicator_config.val * breathe_offset);
    set_indicator_hsv(breathe);

    // Update offset
    float step = 0.02f; // smaller = smoother
    if (breathe_up) {
        breathe_offset += step;
        if (breathe_offset >= 1.0f) {
            breathe_offset = 1.0f;
            breathe_up = false;
        }
    } else {
        breathe_offset -= step;
        if (breathe_offset <= 0.0f) {
            breathe_offset = 0.0f;
            breathe_up = true;
        }
    }

    return indicator_config.animation_speed/BREATHING_SPEEDUP_FACTOR;
}


/* --- MODE SWITCHING AND PROCESSING --- */
void post_init_hooks(void){
    eeconfig_read_kb_datablock(&indicator_config, WB_EECONFIG_DATA_SIZE, EECONFIG_KB_DATA_SIZE-WB_EECONFIG_DATA_SIZE);

    charging_token = defer_exec(500, charge_callback, NULL);
    handle_indicator_oneshot_modes();
}

void handle_indicator_oneshot_modes(void){
    switch (indicator_config.mode){
        case OFF:
            cancel_deferred_exec(dynamic_effect_token);
            set_indicator_hsv((HSV){0,0,0});
            break;
        case STATIC:
            cancel_deferred_exec(dynamic_effect_token);
            set_indicator_hsv(get_indicator_hsv());
            break;
        case GRADIENT:
            cancel_deferred_exec(dynamic_effect_token);
            dynamic_effect_token = defer_exec(indicator_config.animation_speed, gradient_callback, NULL);
            break;
        case BREATHING:
            cancel_deferred_exec(dynamic_effect_token);
            dynamic_effect_token = defer_exec(indicator_config.animation_speed/BREATHING_SPEEDUP_FACTOR, breathing_callback, NULL);
            break;
        default:
            break;
    }
}

bool process_indicator_keycodes(uint16_t keycode, keyrecord_t *record){
    switch(keycode){
        case INDICATOR_MODE:
            if(record -> event.pressed) {
                change_indicator_mode();
            }
            return false;
        case INDICATOR_HUEU:
            if(record -> event.pressed){
                update_indicator_color(INDICATOR_HUE_STEP,0);
            }
            return false;
        case INDICATOR_HUED:
            if(record -> event.pressed){
                update_indicator_color(-INDICATOR_HUE_STEP,0);
            }
            return false;
        case INDICATOR_SPDU:
            if(record -> event.pressed) {
                change_indicator_speed(-INDICATOR_SPEED_STEP);
            }
            return false;
        case INDICATOR_SPDD:
            if(record -> event.pressed) {
                change_indicator_speed(INDICATOR_SPEED_STEP);
            }
            return false;
        case INDICATOR_VALU:
            if(record -> event.pressed) {
                update_indicator_color(0,(int)UINT8_MAX/INDICATOR_BRIGHTNESS_LEVELS);
            }
            return false;
        case INDICATOR_VALD:
            if(record -> event.pressed) {
                update_indicator_color(0,(int)-UINT8_MAX/INDICATOR_BRIGHTNESS_LEVELS);
            }
            return false;
        case BATTERY_LEVEL:
            if(record -> event.pressed){
                battery_indicator_enabled = true;
            } else {
                battery_indicator_enabled = false;
                if(current_mode_requires_update()) handle_indicator_oneshot_modes();
            }
            return false;
        case INDICATOR_CHRG:
            if(record -> event.pressed){
                indicator_config.charging_indicator_enabled = !indicator_config.charging_indicator_enabled;

                if(!indicator_config.charging_indicator_enabled && current_mode_requires_update()) handle_indicator_oneshot_modes();

                eeconfig_update_kb_datablock(&indicator_config, WB_EECONFIG_DATA_SIZE, EECONFIG_KB_DATA_SIZE-WB_EECONFIG_DATA_SIZE);
            }
            return false;
        default:
            return true;
    }
}

void process_indicators(void) {
    if(battery_indicator_enabled) show_battery_indicator();
}


/* --- STATUS: Battery --- */
void show_battery_indicator(void){
    md_inquire_bat();
    uint8_t level = *md_getp_bat();

    rgb_matrix_set_color_all(0,0,0);

    uint8_t battery_leds = level / 10;

    for (uint8_t i = 18; i < battery_leds + 18; i++){
        rgb_matrix_set_color(i, 0,255,0);
    }
}


/* --- STATUS: Charging --- */
static const HSV charge_colors[6] = {
    { 0,   255, 255 },  // red
    { 40,  255, 255 },  // orange
    { 80,  255, 255 },  // yellow
    { 120, 255, 255 },  // green
    { 180, 255, 255 },  // cyan
    { 240, 255, 255 },  // blue
};

uint32_t charge_callback(uint32_t trigger_time, void *cb_arg){
    bool cable_detect = readPin(BT_CABLE_PIN);
    bool completed = readPin(BT_CHARGE_PIN);
    static uint8_t idx = 0;

    // CD status seems to only update after I physically toggle the switch.
    // Working around this, we check for completed in the first if check and don't check for CD in last check.
    if(!cable_detect && !completed){
        chg_status = NOCHG;
    } else if (cable_detect && !completed){
        chg_status = CHARGING;
    } else if (completed){
        chg_status = FULL;
    }

    if(indicator_config.charging_indicator_enabled && !battery_indicator_enabled){
        if(chg_status == CHARGING){
            set_indicator_hsv(charge_colors[idx]);
            idx++;
            if (idx > 5) idx = 0;
        } else if (chg_status == FULL) {
            HSV green = {120, 255, 100};
            set_indicator_hsv(green);
            idx=0;
        }
    }

    return (!(chg_status == NOCHG)) ? 100 : 500; // Increase poll frequency when charging
}
#endif


/* --- UTILITES --- */
HSV get_indicator_hsv(void){
    return (HSV) {indicator_config.hue, 255, indicator_config.val};
}

HSV hsv_blend(HSV c0, HSV c1, float t) {
    HSV blend;
    // hue shortest path
    int16_t dh = c1.h - c0.h;
    if (dh > 127) dh -= 256;
    if (dh < -127) dh += 256;
    int16_t h = c0.h + dh * t;
    if (h < 0) h += 256;
    if (h > 255) h -= 256;
    blend.h = h;
    blend.s = c0.s + (c1.s - c0.s) * t;
    blend.v = c0.v + (c1.v - c0.v) * t;
    return blend;
}

void change_indicator_mode(void) {
    indicator_config.mode += 1;
    if (indicator_config.mode > 3) indicator_config.mode = 0;

    handle_indicator_oneshot_modes();
    eeconfig_update_kb_datablock(&indicator_config, WB_EECONFIG_DATA_SIZE, EECONFIG_KB_DATA_SIZE-WB_EECONFIG_DATA_SIZE);
}

void change_indicator_speed(int ds) {
    indicator_config.animation_speed += ds;

    eeconfig_update_kb_datablock(&indicator_config, WB_EECONFIG_DATA_SIZE, EECONFIG_KB_DATA_SIZE-WB_EECONFIG_DATA_SIZE);
}

bool current_mode_requires_update(void){
    if(indicator_config.mode == STATIC || indicator_config.mode == OFF) return true;
    return false;
}

bool overlay_active(void) {
    if(battery_indicator_enabled || (indicator_config.charging_indicator_enabled && chg_status != NOCHG)) return true;
    return false;
}

void update_indicator_color(int dh, int dv){
    indicator_config.hue += dh;

    int temp_v = indicator_config.val + dv;

    if(temp_v > UINT8_MAX) indicator_config.val = UINT8_MAX;
    else if(temp_v < 0) indicator_config.val = 0;
    else indicator_config.val += dv;

    eeconfig_update_kb_datablock(&indicator_config, WB_EECONFIG_DATA_SIZE, EECONFIG_KB_DATA_SIZE-WB_EECONFIG_DATA_SIZE);

    if(current_mode_requires_update()) handle_indicator_oneshot_modes();
}

void set_indicator_hsv(HSV hsv){
    for (uint8_t i = INDICATOR_LED_START; i < INDICATOR_LED_START+INDICATOR_LED_COUNT; i++) {
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
}
