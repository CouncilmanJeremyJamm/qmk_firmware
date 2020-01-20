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

#pragma   once
#include "config_common.h"

/* Sets WinCompose activation key */
#define  UNICODE_KEY_WINC KC_RALT

/* Set USB polling rate to 1000Hz */
#define  USB_POLLING_INTERVAL_MS 1

/* Forces N-key rollover on */
#define  FORCE_NKRO

/* Require faster taps in tapdance */
#define  TAPPING_TERM 175

/* Disables debouncing */
#undef   DEBOUNCE
#define  DEBOUNCE 0

/* Only enable used RGB animations */
#undef   RGBLIGHT_ANIMATIONS
#define  RGBLIGHT_SLEEP

/* Reduces file size */
#define  NO_ACTION_ONESHOT
#define  NO_ACTION_MACRO
#define  NO_ACTION_FUNCTION
#define  IGNORE_MOD_TAP_INTERRUPT
#define  TAPPING_FORCE_HOLD
