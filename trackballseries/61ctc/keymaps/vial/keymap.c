#include QMK_KEYBOARD_H
#include "ocean_dream.h"
#include "kodama.h"
#include "os_detection.h"

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum custom_keycodes {
    QM_PND = SAFE_RANGE,
    QM_EURO,
    QM_EMD,
    QM_PLCR,
    QM_SLCR,
    QM_BULL
};

enum charybdis_keymap_layers {
    LAYER_MAC = 0,
    LAYER_WIN,
    LAYER_GAME,
    LAYER_MOUSE,
    LAYER_SPECIAL,
    LAYER_FUNCTION,
    LAYER_POINTER,
};

// Pointer options
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;
#endif    // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif    // !POINTING_DEVICE_ENABLE

typedef union {
    uint32_t raw1;
    struct {
        uint16_t auto_time: 13;
        uint8_t threshold_value : 3; 
        bool    is_auto_enabled : 1; 
        bool    is_oled_enabled : 1; 
    };
} auto_config_t;

static auto_config_t user_config;

// OS detection
bool process_detected_host_os_user(os_variant_t detected_os) {
    switch (detected_os) {
        case OS_WINDOWS:
        case OS_LINUX:
            // Turn on LAYER_WIN when Windows/Linux is detected
            layer_on(LAYER_WIN);
            
            // Disable NKRO (switch to 6KRO) for maximum compatibility with KVMs/BIOS
            if (keymap_config.nkro) {
                keymap_config.nkro = 0;
                eeconfig_update_keymap(&keymap_config); // <-- Fixed line
                clear_keyboard();
            }
            break;
            
        case OS_MACOS:
        case OS_IOS:
            // Ensure LAYER_WIN is off to fall back to the default LAYER_MAC
            layer_off(LAYER_WIN);
            
            // Automatically enable NKRO when Mac/iOS is detected
            if (!keymap_config.nkro) {
                keymap_config.nkro = 1;
                eeconfig_update_keymap(&keymap_config);
                clear_keyboard();
            }
            break;
            
        default:
            break;
    }
    return true;
}

void keyboard_post_init_user(void){
    user_config.raw1 = eeconfig_read_user();
}

void eeconfig_init_user(void){
    user_config.raw1 = 0;
    user_config.auto_time = 0;
    user_config.is_auto_enabled = 0;
    user_config.threshold_value = 1;
    user_config.is_oled_enabled = 0;
    eeconfig_update_user(user_config.raw1);
}

#define CHARYBDIS_AUTO_MAX_TIMEOUT_MS 3000
#define CHARYBDIS_AUTO_MIN_TIMEOUT_MS 0

// Charybdis trackball settings
#ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS user_config.auto_time
#endif

#ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD user_config.threshold_value
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD


// tap hold configurations
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LGUI_T(KC_SPACE):
        case LSFT_T(KC_BSLS):
        case LCTL_T(KC_DELETE):
        case LCTL_T(KC_SPACE):
        case LALT_T(KC_BSPC):
        case LT(5, KC_ENTER):
            return 125;
        case LGUI_T(KC_DELETE):
        case LT(6, KC_SCLN):
            return 150;
        default:
            return TAPPING_TERM;
    }
}
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LGUI_T(KC_DELETE):
        case LALT_T(KC_BSPC):
        case RSFT_T(KC_SLSH):
        case LT(6, KC_SCLN):
        case LT(4, KC_Z):
        case LT(4, KC_DOT):
            // Immediately select the hold action when another key is tapped.
            return true;
        default:
            // Do not select the hold action when another key is tapped.
            return false;
    }
}
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LGUI_T(KC_SPACE):
        case LSFT_T(KC_BSLS):
        case LCTL_T(KC_DELETE):
        case LCTL_T(KC_SPACE):
        case LT(5, KC_ENTER):
            // Immediately select the hold action when another key is pressed.
            return true;
        default:
            // Do not select the hold action when another key is pressed.
            return false;
    }
}
bool get_speculative_hold(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) { // These keys may be speculatively held.
        case LGUI_T(KC_SPACE):
        case LSFT_T(KC_BSLS):
        case LCTL_T(KC_DELETE):
        case LCTL_T(KC_SPACE):
        case LT(5, KC_ENTER):
        case LT(6, KC_SCLN):
            return true;
    }
    return false; // Disable otherwise.
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_MAC] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮                           ╭──────────────────────────────────────────────────────╮
MT(MOD_LCTL | MOD_LSFT | MOD_LGUI, KC_ESC), KC_1, KC_2, KC_3,KC_4, KC_5, KC_6, KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
  // ├──────────────────────────────────────────────────────┤                           ├──────────────────────────────────────────────────────┤
MT(MOD_LCTL | MOD_LGUI | MOD_LALT, KC_TAB),    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_BSPC,
  // ├──────────────────────────────────────────────────────┤                           ├──────────────────────────────────────────────────────┤
       KC_LALT,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L, LT(6, KC_SCLN), KC_QUOT,
  // ├──────────────────────────────────────────────────────┤                           ├──────────────────────────────────────────────────────┤
       LSFT_T(KC_BSLS), LT(4, KC_Z), KC_X, KC_C, KC_V, KC_B, LALT_T(KC_BSPC), RCTL_T(KC_DELETE), KC_N, KC_M, KC_COMM, LT(4, KC_DOT), RSFT_T(KC_SLSH), KC_EQUAL,
  // ╰──────────────────────────────────────────────────────┤                           ├──────────────────────────────────────────────────────╯
       KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS, LCTL_T(KC_DELETE), LT(5, KC_ENTER), LGUI_T(KC_SPACE), RGUI_T(KC_SPACE),  LT(5, KC_ENTER), KC_LBRC, KC_RBRC
  //                            ╰───────────────────────────╯                           ╰──────────────────╯
  ),
[LAYER_WIN] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮                   ╭──────────────────────────────────────────────────────╮
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_LCTL, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGUI_T(KC_DELETE), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ╰──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────╯
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, LGUI_T(KC_DELETE), LT(5, KC_ENTER), LCTL_T(KC_SPACE), RCTL_T(KC_SPACE), LT(5, KC_ENTER), KC_TRNS, KC_TRNS                              
  //                            ╰───────────────────────────╯               ╰──────────────────╯
),
  [LAYER_GAME] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮                   ╭──────────────────────────────────────────────────────╮
       KC_ESC, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TAB, KC_A, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_LCTL, KC_Q, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, MS_BTN1, MS_BTN2, DRGSCRL, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_LSFT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS, MS_BTN3, KC_TRNS, MO(4), KC_TRNS, KC_TRNS,
  // ╰──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────╯
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LCTL, KC_TRNS, KC_SPC,             KC_TRNS, KC_TRNS,                    KC_TRNS, KC_TRNS                 
  //                            ╰───────────────────────────╯               ╰──────────────────╯
),
  [LAYER_MOUSE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮                   ╭──────────────────────────────────────────────────────╮
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, MS_BTN1, MS_BTN2, DRGSCRL, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS, MS_BTN3, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ╰──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────╯
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,             KC_TRNS, KC_TRNS,                    KC_TRNS, KC_TRNS                 
  //                            ╰───────────────────────────╯               ╰──────────────────╯
),
  [LAYER_SPECIAL] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮                   ╭──────────────────────────────────────────────────────╮
       KC_GRV, KC_TRNS, KC_TRNS, QM_PND, QM_EURO, QM_SLCR,                      KC_TRNS, QM_PLCR, QM_BULL, KC_TRNS, KC_TRNS, QM_EMD,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_7, KC_8, KC_9, KC_SLSH,                               KC_7, KC_8, KC_9, KC_LBRC, KC_RBRC, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_4, KC_5, KC_6, KC_MINS,                               KC_4, KC_5, KC_6, KC_TRNS, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_1, KC_2, KC_3, KC_EQUAL, KC_LSFT,            KC_LSFT,  KC_1, KC_2, KC_3, KC_TRNS, KC_TRNS, KC_TRNS,
  // ╰──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────╯
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_DOT, KC_0, KC_TRNS,             KC_TRNS, KC_0,                     KC_TRNS, KC_TRNS
  //                            ╰───────────────────────────╯               ╰──────────────────╯
),
  [LAYER_FUNCTION] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮                   ╭──────────────────────────────────────────────────────╮
       KC_F12, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,                                 KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BRIU,                     KC_TRNS, KC_HOME, KC_UP,   KC_END,  KC_MRWD, KC_VOLU,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BRID,                     KC_TRNS, KC_LEFT, KC_DOWN, KC_RGHT, KC_MFFD, KC_VOLD,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPLY, KC_MUTE,
  // ╰──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────╯
       KC_TRNS, TG(2), TG(1), QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS,             KC_TRNS, KC_TRNS,                     KC_TRNS, KC_F15                             
  //                            ╰───────────────────────────╯               ╰──────────────────╯
),
   [LAYER_POINTER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮                   ╭──────────────────────────────────────────────────────╮
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, QK_BOOT,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, DPI_MOD, S_D_MOD, AUTO_TIME_50, AUTO_THRESHOLD, TInfo,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                      KC_TRNS, MS_BTN1, MS_BTN2, DRGSCRL, KC_TRNS, KC_TRNS,
  // ├──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────┤
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     T_SAVE, AUTO_MODE_TOGGLE, MS_BTN3, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  // ╰──────────────────────────────────────────────────────┤                   ├──────────────────────────────────────────────────────╯
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,             KC_TRNS, C(KC_UP),                              KC_TRNS, KC_TRNS                 
  //                            ╰───────────────────────────╯               ╰──────────────────╯
),
};

// clang-format on

#ifdef POINTING_DEVICE_ENABLE

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if(user_config.is_auto_enabled){
            if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
                if (auto_pointer_layer_timer == 0) {
                    layer_on(LAYER_MOUSE);
                }
                auto_pointer_layer_timer = timer_read();
            }
    }

    return mouse_report;
}

void matrix_scan_user(void) {
    if(user_config.is_auto_enabled){
        if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
            auto_pointer_layer_timer = 0;
            layer_off(LAYER_MOUSE);
        };
    }
}
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE


#ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case AUTO_MODE_TOGGLE:
            if(record->event.pressed) {
                user_config.is_auto_enabled ^=  1;
            } 
            return false;
        case AUTO_TIME_50:
            if(record->event.pressed) { 
                user_config.auto_time += 50;
                if(user_config.auto_time > CHARYBDIS_AUTO_MAX_TIMEOUT_MS){
                    user_config.auto_time = CHARYBDIS_AUTO_MIN_TIMEOUT_MS;
                }
            } 
            return false;
        case AUTO_TIME_50R:
            if(record->event.pressed) {
                user_config.auto_time -= 50;
                if(user_config.auto_time > CHARYBDIS_AUTO_MAX_TIMEOUT_MS){
                    user_config.auto_time = CHARYBDIS_AUTO_MAX_TIMEOUT_MS;
                }
            } 
            return false; 
        case AUTO_TIME_100:
            if(record->event.pressed) { 
                user_config.auto_time += 100;
                if(user_config.auto_time > CHARYBDIS_AUTO_MAX_TIMEOUT_MS){
                    user_config.auto_time = 0;
                }
            }
            return false;
        case AUTO_THRESHOLD:
            if(record->event.pressed) {
                user_config.threshold_value +=  1;
            } 
            return false;
        case TInfo:
            if(record->event.pressed) {
                user_config.is_oled_enabled ^=  1;
            }
            return false; 
        case T_SAVE:
            if(record->event.pressed) { 
                eeconfig_update_user(user_config.raw1);
            }
            return false; 
        case QM_PND: // Alt + 0163
            SEND_STRING(SS_LALT(SS_TAP(X_KP_0)SS_TAP(X_KP_1)SS_TAP(X_KP_6)SS_TAP(X_KP_3)));
            return false;
        case QM_EURO: // Alt + 0128
            SEND_STRING(SS_LALT(SS_TAP(X_KP_0)SS_TAP(X_KP_1)SS_TAP(X_KP_2)SS_TAP(X_KP_8)));
            return false;
        case QM_EMD: // Alt + 0151
            SEND_STRING(SS_LALT(SS_TAP(X_KP_0)SS_TAP(X_KP_1)SS_TAP(X_KP_5)SS_TAP(X_KP_1)));
            return false;
        case QM_PLCR: // Alt + 0182
            SEND_STRING(SS_LALT(SS_TAP(X_KP_0)SS_TAP(X_KP_1)SS_TAP(X_KP_8)SS_TAP(X_KP_2)));
            return false;
        case QM_SLCR: // Alt + 0167
            SEND_STRING(SS_LALT(SS_TAP(X_KP_0)SS_TAP(X_KP_1)SS_TAP(X_KP_6)SS_TAP(X_KP_7)));
            return false;
        case QM_BULL: // Alt + 7
            SEND_STRING(SS_LALT(SS_TAP(X_KP_7)));
            return false;
        // activates is_calm for ocean dream animation
        case KC_LCTL:
        case KC_RCTL:
            #ifdef OCEAN_DREAM_ENABLE
                        is_calm = (record->event.pressed) ? true : false;
            #endif
            break;
    }
    return true;
}

/* oled stuff :) */
#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
     if (is_keyboard_master()){
        if (user_config.is_oled_enabled) {
            return OLED_ROTATION_0;    // TInfo Mode
        } else {
            return OLED_ROTATION_270;  // Ocean Dream Mode
        }
    } else {
        return OLED_ROTATION_270;      // Kodama Animation (Vertical)
    }
}

// Slave Device Data
static void slave_data(void) {
    /* Print current layer at the bottom */
    oled_set_cursor(0, 11);
    oled_write("", false);

    switch (get_highest_layer(layer_state)) {
        case LAYER_MAC:
            oled_write("     ", false);
            break;
        case LAYER_WIN:
            oled_write(" WIN ", false);
            break;
        case LAYER_GAME:
            oled_write(":GAME", false);
            break;
        case LAYER_MOUSE:
            oled_write("TRACK", false);
            break;
        case LAYER_SPECIAL:
            oled_write(" :EX ", false);
            break;
        case LAYER_FUNCTION:
            oled_write(" :Fn ", false);
            break;
        case LAYER_POINTER:
            oled_write("SNIPE", false);
            break;
        default:
            oled_write("Undef", false);
    }
    
    // Render the Kodama animation on the slave half
    animate_kodama(); 
}

static void tv_ms(void) {
    // 鼠标模式OLED
    oled_write_P(PSTR("AutoL:"), false);
    char auto_m[2];
    snprintf(auto_m, sizeof(auto_m), "%d", user_config.is_auto_enabled);
    oled_write(auto_m, false);
    trackball_oled_default();
    
    // 自动切层模式
    oled_write_P(PSTR("ATV  :"), false);
    char count_atv_str[2];
    snprintf(count_atv_str, sizeof(count_atv_str), "%d", CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD);
    oled_write(count_atv_str, false);

    oled_write_P(PSTR(" AUTO-MS:"), false);
    char count_ams_str[6];
    snprintf(count_ams_str, sizeof(count_ams_str), "%d", CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS);
    oled_write_ln(count_ams_str, false);

    // 滚动/阻击模式OLED
    trackball_oled_info();

    #if OLED_TIMEOUT > 0
        if (last_input_activity_elapsed() > OLED_TIMEOUT) {
            oled_clear();
            oled_off();
            return;
    } else {
        oled_on();
    }
    #endif
}

// Master Device OLED
static void master_data(void) {
    static bool last_oled_state = 2; // Initialize to OPPOSITE to force refresh on boot

    // If the OLED mode changed, update rotation and clear
    if (user_config.is_oled_enabled != last_oled_state) {
        if (user_config.is_oled_enabled) {
            // Text mode
            oled_init(OLED_ROTATION_0);   // horizontal
        } else {
            // Animation mode
            oled_init(OLED_ROTATION_270); // vertical
        }
        oled_clear();
        last_oled_state = user_config.is_oled_enabled;
    }

    // Render current mode
    if (user_config.is_oled_enabled) {
        tv_ms();   // ensure this function does NOT call oled_clear()
    } else {
        #ifdef OCEAN_DREAM_ENABLE
            render_stars();
        #endif
    }
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        master_data();
    } else {
        slave_data();
    }
    return false;
}
#endif