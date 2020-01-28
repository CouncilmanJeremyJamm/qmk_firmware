/* Copyright 2018 Carlos Filoteo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

#define LT3_TAB     LT(3, KC_TAB)
#define BASE_COLOUR HSV_CORAL

/* ---------- UNDERGLOW ---------- */

/* Sets the default underglow colour on boot */
void keyboard_post_init_user(void) {
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(BASE_COLOUR);
    wait_ms(20); //This delay is ESSENTIAL, not sure why.
    rgblight_mode_noeeprom(1);
}

/* Names layers in sequential order */
enum layers {
    _BASE,
    _NUMS,
    _SYMB,
    _MED,
    _UNIC
};

/* Assigns a unique underglow colour to each layer */
layer_state_t layer_state_set_user(layer_state_t state) {
    switch(get_highest_layer(state)) {
    case _MED:
        rgblight_sethsv_noeeprom(HSV_YELLOW);
        break;
    case _SYMB:
        rgblight_sethsv_noeeprom(HSV_MAGENTA);
        break;
    case _NUMS:
        rgblight_sethsv_noeeprom(HSV_TEAL);
        break;
    case _UNIC:
        rgblight_sethsv_noeeprom(HSV_SPRINGGREEN);
        break;
    default:
        rgblight_sethsv_noeeprom(BASE_COLOUR);
        break;
    }
  	return state;
}

/* ---------- UNICODE ---------- */

enum keycodes {
    KC_UNIC = SAFE_RANGE,
    UNICODE_RANGE //Must be last
};

#define Y(i) (UNICODE_RANGE + i) //Unicode chars are stored by index (above UNICODE_RANGE)
#define char_pair(c, o) (((c##UL + o) << 0x10) + c) //Concat code + offset code

enum unicode_index {
    ALPH, //α
    BETA, //β
    GAMM, //γ
    DELT, //δ
    EPSI, //ε
    THET, //θ
    LAMB, //λ
    MU,   //μ
    NU,   //ν
    PI,   //π
    RHO,  //ρ
    SIGM, //σ
    OMEG, //ω
    CHARS_END //Length of enum, do not remove
};

const uint32_t PROGMEM unicode_chars[] = {
    [ALPH] = char_pair(0x03B1,-0x20),
    [BETA] = char_pair(0x03B2,-0x20),
    [GAMM] = char_pair(0x03B3,-0x20),
    [DELT] = char_pair(0x03B4,-0x20),
    [EPSI] = char_pair(0x03B5,-0x20),
    [THET] = char_pair(0x03B8,-0x20),
    [LAMB] = char_pair(0x03BB,-0x20),
    [MU]   = char_pair(0x03BC,-0x20),
    [NU]   = char_pair(0x03BD,-0x20),
    [PI]   = char_pair(0x03C0,-0x20),
    [RHO]  = char_pair(0x03C1,-0x20),
    [SIGM] = char_pair(0x03C3,-0x20),
    [OMEG] = char_pair(0x03C9,-0x20)
};

/* Converts each hex digit to the literal character, e.g. A -> 'A' */
uint16_t hex_to_kc(uint8_t digit) {
    if (digit == 0x0) {
        return KC_0;
    } else if (digit < 0xA) {
        return KC_1 + (digit - 0x1);
    } else {
        return KC_A + (digit - 0xA);
    }
}

/* Taps hex code as it is read */
void tap_hex(uint16_t hex) {
    uint8_t mods = get_mods();
    clear_mods();
    for (int i = 3; i >= 0; i--) {
        tap_code(hex_to_kc((hex >> (i * 4)) & 0xF));
    }
    set_mods(mods);
}

/* Processing of unicode charaacters */
bool process_record_unicode(uint16_t keycode, keyrecord_t *record) {
    if ((keycode >= UNICODE_RANGE) && (keycode < (UNICODE_RANGE + CHARS_END))) {
        if (record->event.pressed) {
            uint32_t code = pgm_read_dword(unicode_chars + (keycode - UNICODE_RANGE));
            tap_code(UNICODE_KEY_WINC);
            tap_code(KC_U);
            tap_hex(((get_mods() & MOD_MASK_SHIFT) ? code >> 0x10 : code) & 0xFFFF);
            tap_code(KC_ENTER);
        }
        return false; // Prevent further processing of unicode keycodes
    } else {
        return true; // Other keycodes are processed normally
    }
}

bool layer_key_pressed = false;
uint16_t layer_key_timer;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) { // Functionality for custom keycodes
    case KC_UNIC:
        if (record->event.pressed) {
            if (layer_state & (1UL << _UNIC)) {
                layer_off(_UNIC);
            } else {
                layer_key_pressed = true;
                layer_key_timer = timer_read();
            }
        } else {
            layer_key_pressed = false;
        }
        return false;
    default:
        return process_record_unicode(keycode, record); // Pass other keycodes to the unicode handler
    }
}

void matrix_scan_user(void) {
    if (layer_key_pressed) {
        if (timer_elapsed(layer_key_timer) >= 800) {
            layer_on(_UNIC);
            tap_code(KC_RALT);
            tap_code(KC_ENT);
        }
    }
}

/* ---------- Tapdance ---------- */
enum tapdances {
    LBRACS,
    RBRACS,
    BTLDR
};

/* Shortened tapdance keycodes */
#define TD_LBR  TD(LBRACS)
#define TD_RBR  TD(RBRACS)
#define TD_RSET TD(BTLDR)

void lbracs_tapdance(qk_tap_dance_state_t *state, void *user_data) {
	switch(state->count) {
    case 1:
        SEND_STRING("(");
        break;
    case 2:
        SEND_STRING("[");
        break;
    case 3:
        SEND_STRING("{");
        break;
    }
}

void rbracs_tapdance(qk_tap_dance_state_t *state, void *user_data) {
    switch(state->count) {
    case 1:
        SEND_STRING(")");
        break;
    case 2:
        SEND_STRING("]");
        break;
    case 3:
		SEND_STRING("}");
        break;
    }
}

/* Reset button only activates on a triple tap */
void tdreset_tapdance(qk_tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
	    rgblight_sethsv_noeeprom (HSV_RED); // Red colour indicates bootloader mode
	    reset_keyboard();
	}
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [LBRACS] = ACTION_TAP_DANCE_FN(lbracs_tapdance),
    [RBRACS] = ACTION_TAP_DANCE_FN(rbracs_tapdance),
    [BTLDR]  = ACTION_TAP_DANCE_FN(tdreset_tapdance)
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Base Layer
 * ,-------------------------------------------------------------------------.
 * | Esc |  Q  |  W  |  E  |  R  |  T  |  Y  |  U  |  I  |  O  |  P  |Bspace |
 * |-------------------------------------------------------------------------+
 * |Tab/L3|  A  |  S  |  D  |  F  |  G  |  H  |  J  |  K  |  L  |  '  |   ;  |
 * |-------------------------------------------------------------------------+
 * | Shift |  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |Sh/En|
 * |-------------------------------------------------------------------------+
 * | Ctrl| Gui | Alt | App |  L1 |   Space    |  L2  | Left| Down|  Up |Right|
 * `-------------------------------------------------------------------------'
 */

    [_BASE] = LAYOUT(
      KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
      LT3_TAB, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT, KC_SCLN,
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_SFTENT,
      KC_LCTL, KC_LGUI, KC_LALT, KC_UNIC, TT(1),       KC_SPC,       MO(2),   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

/* FN Layer 1: hexadecimal and numpad
 * ,-------------------------------------------------------------------------.
 * | ` ~ |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  0  |       |
 * |-------------------------------------------------------------------------+
 * |      |  A  |  B  |  C  |  D  |  E  |  F  |  4  |  5  |  6  |  -  |   \  |
 * |-------------------------------------------------------------------------+
 * |       |     |     |     |     |     |     |  1  |  2  |  3  |  =  |     |
 * |-------------------------------------------------------------------------+
 * |     |     |     |     |     |            |      |     |     |     |     |
 * `-------------------------------------------------------------------------'
 */

    [_NUMS] = LAYOUT( /* Left */
      KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
      _______, KC_A,    KC_B,    KC_C,    KC_D,    KC_E,    KC_F,    KC_4,    KC_5,    KC_6,    KC_MINS, KC_BSLS,
      _______, _______, _______, _______, _______, _______, _______, KC_1,    KC_2,    KC_3,    KC_EQL,  _______,
      _______, _______, _______, _______, TG(1),     _______,      _______, _______, _______, _______, _______
    ),

/* FN Layer 2: symbols and brackets
 * ,-------------------------------------------------------------------------.
 * |  ~  |  !  |  "  |  @  |  $  |  %  |  ^  |  &  |  *  |  -  |  =  |       |
 * |-------------------------------------------------------------------------+
 * |      | ([{ |     | )]} |     |     |     |     |  _  |  +  |     |   |  |
 * |-------------------------------------------------------------------------+
 * |       |     |     |     |     |     |     |     |     |     |     |     |
 * |-------------------------------------------------------------------------+
 * |     |     |     |     |     |            |      |     |     |     |     |
 * `-------------------------------------------------------------------------'
 */

    [_SYMB] = LAYOUT( /* Right */
      KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_MINS, KC_EQL,  _______,
      _______, TD_LBR,  _______, TD_RBR,  _______, _______, _______, _______, KC_UNDS, KC_PLUS, _______, KC_PIPE,
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, _______,     _______,      _______, _______, _______, _______, _______
    ),

/* FN Layer 3: media and navigation
 * ,-------------------------------------------------------------------------.
 * | Esc | F1  | F2  | F3  | F4  | F5  | F6  | F7  | F8  | F9  | F10 | Delete|
 * |-------------------------------------------------------------------------+
 * |      | RGB |     |     |     |     |     | Prev| Play| Next|     |      |
 * |-------------------------------------------------------------------------+
 * |       |     |     |     |     |RESET|     |     |     |     |     |     |
 * |-------------------------------------------------------------------------+
 * |     |     |     |     |     |            |      | Home| PgDn| PgUp| End |
 * `-------------------------------------------------------------------------'
 */

    [_MED] = LAYOUT( /* Tab */
      KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_DEL,
      _______, RGB_TOG, _______, _______, _______, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, _______, _______,
      _______, _______, _______, _______, _______, TD_RSET, _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, _______,     _______,      _______, KC_HOME, KC_PGDN, KC_PGUP, KC_END
    ),

/* FN Layer 4: Greek/physics symbols
 * ,-------------------------------------------------------------------------.
 * |     |     |     |  ε  |  ρ  |  θ  |     |     |     |  ω  |  π  |       |
 * |-------------------------------------------------------------------------+
 * |      |  α  |  σ  |  δ  |     |  γ  |     |     |     |     |  λ  |      |
 * |-------------------------------------------------------------------------+
 * |       |     |     |     |     |  β  |  ν  |  μ  |     |     |     |     |
 * |-------------------------------------------------------------------------+
 * |     |     |     |     |     |            |      |     |     |     |     |
 * `-------------------------------------------------------------------------'
 */

    [_UNIC] = LAYOUT( /* Tab */
      _______, _______, _______, Y(EPSI), Y(RHO),  Y(THET), _______, _______, _______, Y(OMEG), Y(PI),   _______,
      _______, Y(ALPH), Y(SIGM), Y(DELT), _______, Y(GAMM), _______, _______, _______, Y(LAMB), _______, _______,
      _______, _______, _______, _______, _______, Y(BETA), Y(NU),   Y(MU),   _______, _______, _______, _______,
      _______, KC_UNIC, _______, _______, _______,     _______,      _______, _______, _______, _______, _______
    ),
};
