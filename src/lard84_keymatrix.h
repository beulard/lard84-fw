/*
** file: lard84_keymatrix.h
** author: beulard (Matthias Dubouchet)
** creation date: 09/05/2025
*/

#ifndef _LARD84_KEYMATRIX_H
#define _LARD84_KEYMATRIX_H

#include "pico/types.h"
#include <pico/mutex.h>

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

#define N_ROWS 6
#define N_COLS 16

// Setup GPIOs of the key matrix
void l84_keymatrix_setup();
// Query the state of all keys on the keyboard
// Mutex should be acquired for read/write access to the keymatrix state
void l84_keymatrix_poll(mutex_t *mutex);
// Print out what keys are pressed according to the last call
// to l84_keymatrix_update
void l84_keymatrix_report();
// Returns true if switch at index is pressed down
bool l84_keymatrix_is_key_pressed(uint col, uint row);
// Returns true if the Fn/layer key is pressed
bool l84_keymatrix_is_fn_key_pressed();

#endif /* _LARD84_KEYMATRIX_H */