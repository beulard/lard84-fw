/*
** file: lard84_keycodes.h
** author: beulard (Matthias Dubouchet)
** creation date: 10/05/2025
**
** Convert from lard84 keymatrix index to HID keycodes.
*/

#ifndef _LARD84_KEYCODES_H
#define _LARD84_KEYCODES_H

#include <stdint.h>

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

uint32_t l84_keycode_get(uint8_t col, uint8_t row, bool fn_layer);

#endif /* _LARD84_KEYCODES_H */