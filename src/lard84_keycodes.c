/*
** file: lard84_keycodes.c
** author: beulard (Matthias Dubouchet)
** creation date: 11/05/2025
**
** Convert from lard84 keymatrix index to HID keycodes.
*/

#include "class/hid/hid.h"
#include "lard84_keymatrix.h"

// HID keycode associated to each key
// Differences with usual ANSI layout:
// None ? TODO(mdu)
// Note row,col order instead of col,row in lard84_keymatrix
const uint32_t l84_hid_keycode[N_ROWS][N_COLS] = {
    {
        HID_KEY_ESCAPE,
        HID_KEY_F1,
        HID_KEY_F2,
        HID_KEY_F3,
        HID_KEY_F4,
        HID_KEY_F5,
        HID_KEY_F6,
        HID_KEY_F7,
        HID_KEY_F8,
        HID_KEY_F9,
        HID_KEY_F10,
        HID_KEY_F11,
        HID_KEY_F12,
        HID_KEY_PRINT_SCREEN,
        HID_KEY_INSERT,
        HID_KEY_DELETE,
    },
    {
        HID_KEY_GRAVE,
        HID_KEY_1,
        HID_KEY_2,
        HID_KEY_3,
        HID_KEY_4,
        HID_KEY_5,
        HID_KEY_6,
        HID_KEY_7,
        HID_KEY_8,
        HID_KEY_9,
        HID_KEY_0,
        HID_KEY_MINUS,
        HID_KEY_EQUAL,
        HID_KEY_BACKSPACE,
        HID_KEY_NONE,
        HID_KEY_HOME,
    },
    {
        HID_KEY_TAB,
        HID_KEY_Q,
        HID_KEY_W,
        HID_KEY_E,
        HID_KEY_R,
        HID_KEY_T,
        HID_KEY_Y,
        HID_KEY_U,
        HID_KEY_I,
        HID_KEY_O,
        HID_KEY_P,
        HID_KEY_BRACKET_LEFT,
        HID_KEY_BRACKET_RIGHT,
        HID_KEY_BACKSLASH,
        HID_KEY_NONE,
        HID_KEY_PAGE_UP,
    },
    {
        HID_KEY_CAPS_LOCK,
        HID_KEY_A,
        HID_KEY_S,
        HID_KEY_D,
        HID_KEY_F,
        HID_KEY_G,
        HID_KEY_H,
        HID_KEY_J,
        HID_KEY_K,
        HID_KEY_L,
        HID_KEY_SEMICOLON,
        HID_KEY_APOSTROPHE,
        HID_KEY_NONE,
        HID_KEY_ENTER,
        HID_KEY_NONE,
        HID_KEY_PAGE_DOWN,
    },
    {
        HID_KEY_SHIFT_LEFT,
        HID_KEY_NONE,
        HID_KEY_Z,
        HID_KEY_X,
        HID_KEY_C,
        HID_KEY_V,
        HID_KEY_B,
        HID_KEY_N,
        HID_KEY_M,
        HID_KEY_COMMA,
        HID_KEY_PERIOD,
        HID_KEY_SLASH,
        HID_KEY_NONE,
        HID_KEY_SHIFT_RIGHT,
        HID_KEY_ARROW_UP,
        HID_KEY_END,
    },
    {
        // Row 4: index 53-69
        HID_KEY_CONTROL_LEFT,
        HID_KEY_GUI_LEFT,
        HID_KEY_ALT_LEFT,
        HID_KEY_NONE,
        HID_KEY_NONE,
        HID_KEY_NONE,
        HID_KEY_SPACE,
        HID_KEY_NONE,
        HID_KEY_NONE,
        HID_KEY_NONE,
        HID_KEY_ALT_RIGHT,
        HID_KEY_NONE, // Function/layer key, handled in
                      // l84_keymatrix_is_fn_key_pressed
        HID_KEY_CONTROL_RIGHT,
        HID_KEY_ARROW_LEFT,
        HID_KEY_ARROW_DOWN,
        HID_KEY_ARROW_RIGHT,
    },
};

// Keycode associated with each key, when the function key is also pressed
// Differences with the table above:
// - None (TODO(mdu))
// On lard61, here were the differences:
// - Backtick (GRAVE) on the top left ESC key
// - F1-F12 keys on the top layer
// - Directional arrows on WASD, PG_UP on Q, PG_DOWN on E
// - Directional arrows on PL:" for one-handed motions
// - Delete key on backspace
// - HOME on R, END on F
// - Caps lock on the physical caps lock key
// - Escape on the top left key (tilde)
const uint32_t l84_hid_keycode_fn[N_ROWS][N_COLS] = {
    {
        HID_KEY_ESCAPE,
        HID_KEY_F1,
        HID_KEY_F2,
        HID_KEY_F3,
        HID_KEY_F4,
        HID_KEY_F5,
        HID_KEY_F6,
        HID_KEY_F7,
        HID_KEY_F8,
        HID_KEY_F9,
        HID_KEY_F10,
        HID_KEY_F11,
        HID_KEY_F12,
        HID_KEY_PRINT_SCREEN,
        HID_KEY_INSERT,
        HID_KEY_DELETE,
    },
    {
        HID_KEY_GRAVE,
        HID_KEY_1,
        HID_KEY_2,
        HID_KEY_3,
        HID_KEY_4,
        HID_KEY_5,
        HID_KEY_6,
        HID_KEY_7,
        HID_KEY_8,
        HID_KEY_9,
        HID_KEY_0,
        HID_KEY_MINUS,
        HID_KEY_EQUAL,
        HID_KEY_BACKSPACE,
        HID_KEY_NONE,
        HID_KEY_HOME,
    },
    {
        HID_KEY_TAB,
        HID_KEY_Q,
        HID_KEY_W,
        HID_KEY_E,
        HID_KEY_R,
        HID_KEY_T,
        HID_KEY_Y,
        HID_KEY_U,
        HID_KEY_I,
        HID_KEY_O,
        HID_KEY_P,
        HID_KEY_BRACKET_LEFT,
        HID_KEY_BRACKET_RIGHT,
        HID_KEY_BACKSLASH,
        HID_KEY_NONE,
        HID_KEY_PAGE_UP,
    },
    {
        HID_KEY_CAPS_LOCK,
        HID_KEY_A,
        HID_KEY_S,
        HID_KEY_D,
        HID_KEY_F,
        HID_KEY_G,
        HID_KEY_H,
        HID_KEY_J,
        HID_KEY_K,
        HID_KEY_L,
        HID_KEY_SEMICOLON,
        HID_KEY_APOSTROPHE,
        HID_KEY_NONE,
        HID_KEY_ENTER,
        HID_KEY_NONE,
        HID_KEY_PAGE_DOWN,
    },
    {
        HID_KEY_SHIFT_LEFT,
        HID_KEY_NONE,
        HID_KEY_Z,
        HID_KEY_X,
        HID_KEY_C,
        HID_KEY_V,
        HID_KEY_B,
        HID_KEY_N,
        HID_KEY_M,
        HID_KEY_COMMA,
        HID_KEY_PERIOD,
        HID_KEY_SLASH,
        HID_KEY_NONE,
        HID_KEY_SHIFT_RIGHT,
        HID_KEY_ARROW_UP,
        HID_KEY_END,
    },
    {
        // Row 4: index 53-69
        HID_KEY_CONTROL_LEFT,
        HID_KEY_GUI_LEFT,
        HID_KEY_ALT_LEFT,
        HID_KEY_NONE,
        HID_KEY_NONE,
        HID_KEY_NONE,
        HID_KEY_SPACE,
        HID_KEY_NONE,
        HID_KEY_NONE,
        HID_KEY_NONE,
        HID_KEY_ALT_RIGHT,
        HID_KEY_NONE, // Function/layer key, handled in
                      // l84_keymatrix_is_fn_key_pressed
        HID_KEY_CONTROL_RIGHT,
        HID_KEY_ARROW_LEFT,
        HID_KEY_ARROW_DOWN,
        HID_KEY_ARROW_RIGHT,
    },
};

uint32_t l84_keycode_get(uint8_t col, uint8_t row, bool fn_layer) {
  if (fn_layer)
    return l84_hid_keycode_fn[row][col];
  else
    return l84_hid_keycode[row][col];
}
