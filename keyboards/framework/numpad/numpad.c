// Copyright 2023 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "numpad.h"

void keyboard_post_init_user(void) {
    // Sync initial numlock state from the host
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> parent of cf01a4f167 (keyboards/framework/numpad/keymaps/default/keymap.c)
=======
>>>>>>> parent of 533b389b97 (Merge branch 'fl16-bisect')
    if (host_keyboard_led_state().num_lock) {
        layer_on(_NUMLOCK);
    } else {
        layer_off(_FN);
    }
<<<<<<< HEAD
<<<<<<< HEAD
=======
    //if (host_keyboard_led_state().num_lock) {
        //layer_on(_NUMLOCK);
    //} else {
        //layer_off(_FN);
    //}
>>>>>>> fl16-bisect
=======
>>>>>>> parent of cf01a4f167 (keyboards/framework/numpad/keymaps/default/keymap.c)
=======
>>>>>>> parent of 533b389b97 (Merge branch 'fl16-bisect')
}
