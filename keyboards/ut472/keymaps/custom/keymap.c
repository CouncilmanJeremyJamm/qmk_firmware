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

#define LT3_TAB LT(3, KC_TAB)
#define BASE_COLOUR HSV_CORAL

/* Names layers in sequential order */
enum layers {
    _BASE,
    _NUMS,
    _SYMB,
    _MED,
};

/* Sets the default underglow colour on boot */
void keyboard_post_init_user(void) {
  rgblight_enable_noeeprom();
  rgblight_sethsv_noeeprom(BASE_COLOUR);
  wait_ms(20); //This delay is ESSENTIAL, not sure why.
  rgblight_mode_noeeprom(1);
}

/* Assigns a unique underglow colour to each layer */
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
    case _MED:
        rgblight_sethsv_noeeprom(HSV_YELLOW);
        break;
    case _SYMB:
        rgblight_sethsv_noeeprom(HSV_MAGENTA);
        break;
    case _NUMS:
        rgblight_sethsv_noeeprom(HSV_TEAL);
        break;
    default:
        rgblight_sethsv_noeeprom(BASE_COLOUR);
        break;
    }
  	return state;
}

/* Tapdance */
enum tapdances {
    LBRACS,
    RBRACS,
    TDRESET
};

void lbracs_tapdance(qk_tap_dance_state_t *state, void *user_data) {
	switch (state->count) {
    case 1:
        SEND_STRING ("(");
        break;
    case 2:
        SEND_STRING ("[");
        break;
    case 3:
        SEND_STRING ("{");
        break;
    }
}

void rbracs_tapdance(qk_tap_dance_state_t *state, void *user_data) {
	switch (state->count) {
    case 1:
        SEND_STRING (")");
        break;
    case 2:
        SEND_STRING ("]");
        break;
    case 3:
		    SEND_STRING ("}");
        break;
    }
}

/* Reset button only activates on a triple tap */
void tdreset_tapdance(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 3) {
		    rgblight_sethsv_noeeprom (HSV_RED); // Red colour indicates bootloader mode
		    reset_keyboard();
	  }
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [LBRACS] = ACTION_TAP_DANCE_FN (lbracs_tapdance),
    [RBRACS] = ACTION_TAP_DANCE_FN (rbracs_tapdance),
    [TDRESET] = ACTION_TAP_DANCE_FN (tdreset_tapdance)
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
 * | Ctrl| Gui | Alt | App |  L1 |   Space   |  L2  | Left| Down|  Up  |Right|
 * `-------------------------------------------------------------------------'
 */

    [_BASE] = LAYOUT(
      KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
      LT3_TAB, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT, KC_SCLN,
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_SFTENT,
      KC_LCTL, KC_LGUI, KC_LALT, KC_MPLY, MO(1),       KC_SPC,       MO(2),   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

/* FN Layer 1, numbers and useful symbols
 * ,-------------------------------------------------------------------------.
 * | ` ~ |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  0  | Delete|
 * |-------------------------------------------------------------------------+
 * |      |     |     |     |     |     |     |  -  |  _  |  =  |  +  |  \   |
 * |-------------------------------------------------------------------------+
 * |       |     |     |     |     |     |     |     |     |     |     |     |
 * |-------------------------------------------------------------------------+
 * |     |     |     |     |      |          |       |     |     |     |     |
 * `-------------------------------------------------------------------------'
 */

    [_NUMS] = LAYOUT( /* Left */
      KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DELETE,
      _______, _______, _______, _______, _______, _______, _______, KC_MINS, S(KC_MINS),KC_EQL,S(KC_EQL),KC_BSLS,
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, _______,     _______,      _______, _______, _______, _______, _______
    ),

/* FN Layer 2, symbols and F-keys
 * ,-------------------------------------------------------------------------.
 * |   ~  |  !  |  "  |  #  |  $  |  %  |  ^  |  &  |  *  | ([{ | )]} |Delete|
 * |-------------------------------------------------------------------------+
 * |      | F1  | F2  | F3  |  F4 |  F5 |  F6 |  F7 | F8  | F9  |  F10 |  |  |
 * |-------------------------------------------------------------------------+
 * |       | F11 | F12 | F13 | F14 | F15 | F16 | F17 | F18 | F19 | F20 |     |
 * |-------------------------------------------------------------------------+
 * |     |     |     |Capsl|      |          |       |     |     |     |     |
 * `-------------------------------------------------------------------------'
 */

    [_SYMB] = LAYOUT( /* Right */
      KC_TILDE,  KC_EXCLAIM, KC_AT,  KC_HASH,  KC_DOLLAR, KC_PERCENT, KC_CIRCUMFLEX, KC_AMPERSAND, KC_ASTERISK, TD(LBRACS), TD(RBRACS), KC_DELETE,
      _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10, KC_PIPE,
      _______, KC_F11,   KC_F12,   KC_F13,   KC_F14,   KC_F15,   KC_F16,   KC_F17,   KC_F18,   KC_F19,   KC_F20,  _______,
      _______, _______, _______, KC_CAPS, _______,     _______,      _______, _______, _______, _______, _______
    ),

/* FN Layer 3, media and navigation
 * ,-------------------------------------------------------------------------.
 * | Esc |     |     |     |     |     |     |     | Prev| Play| Next |      |
 * |-------------------------------------------------------------------------+
 * |      |     |     |     |     |     |     |     |     |     |     |      |
 * |-------------------------------------------------------------------------+
 * |       |     |     |     |     |     |     |     |     |     |     |     |
 * |-------------------------------------------------------------------------+
 * |     |     |     |     |      |          |       | Home| PgDn| PgUp| End |
 * `-------------------------------------------------------------------------'
 */

    [_MED] = LAYOUT( /* Tab */
      KC_ESC,  _______, _______, _______, _______, _______, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, _______,
      _______, RGB_TOG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, _______,TD(TDRESET), _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, _______,     _______,      _______, KC_HOME, KC_PGDN, KC_PGUP, KC_END
    ),
};
