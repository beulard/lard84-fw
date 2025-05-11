/*
** file: lard84_keymatrix.c
** author: beulard (Matthias Dubouchet)
** creation date: 08/05/2025
*/

#include "lard84_keymatrix.h"

#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/types.h"
#include <pico/mutex.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

// Minimum time between a key being first pressed/released and the
// moment it is registered
#define DEBOUNCE_THRESHOLD_CYCLES 5

// The following arrays are slightly larger than they need to be: we only
// have 84 physical keys. This is just more convenient for looping over
// rows and columns.

// Keys that are registered as pressed, after debouncing
bool pressed[N_COLS][N_ROWS] = {0};

// Number of l84_keymatrix_poll calls that a given key is pressed.
// When the number exceeds DEBOUNCE_THRESHOLD_CYCLES, the key
// registers as pressed.
uint num_pressed_cycles[N_COLS][N_ROWS] = {0};

// Position of the Fn key
#define L84_FN_COL 11
#define L84_FN_ROW 5

// GPIO pins for each row
static const uint row_pin[N_ROWS] = {
    1,  // row1 /* NOTE(mdu) enable uart on pins 0, 1 by setting this to 30
    29, // row2
    2,  // row3
    28, // row4
    27, // row5
    26, // row6
};

// GPIO pins for each column
static const uint col_pin[N_COLS] = {
    18, // col1
    19, // col2
    20, // col3
    21, // col4
    22, // col5
    23, // col6
    24, // col7
    25, // col8
    3,  // col9
    5,  // col10
    6,  // col11
    7,  // col12
    8,  // col13
    9,  // col14
    10, // col15
    11, // col16
};

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void l84_keymatrix_setup() {
  // Set all row pins as input
  for (uint row = 0; row < N_ROWS; ++row) {
    gpio_init(row_pin[row]);
    printf("Row input: pin %d\n", row_pin[row]);
  }

  // Set all column pins as output
  for (uint col = 0; col < N_COLS; ++col) {
    gpio_init(col_pin[col]);
    gpio_set_dir(col_pin[col], GPIO_OUT);
    printf("Col output: pin %d\n", col_pin[col]);
  }

  printf("Key matrix pins configured\n");
}

void l84_keymatrix_poll(mutex_t *mutex) {
  // Turn each column high, poll rows

  mutex_enter_blocking(mutex);

  for (uint col = 0; col < N_COLS; ++col) {
    // Send the high signal in the active column
    gpio_put(col_pin[col], true);

    for (uint row = 0; row < N_ROWS; ++row) {
      // NOTE(mdu) enabling and disabling the input on our row pins
      // is necessary due to Errata E9 on the RP2350 datasheet.
      // Basically, an input pin driven high will stick high, and
      // the documented way to fix this in software is to reset the
      // input-enabled state.
      gpio_set_input_enabled(row_pin[row], true);
      bool state = gpio_get(row_pin[row]);
      gpio_set_input_enabled(row_pin[row], false);

      if (state) {
        num_pressed_cycles[col][row] += 1;

        if (num_pressed_cycles[col][row] > DEBOUNCE_THRESHOLD_CYCLES) {
          pressed[col][row] = true;
        }
      } else {
        num_pressed_cycles[col][row] = 0;
        pressed[col][row] = false;
      }
    }

    gpio_put(col_pin[col], false);

    // Sufficient for this column to be low before the next iteration
    // Without this, we get false inputs on the next column.
    sleep_us(2);
  }

  mutex_exit(mutex);
}

void l84_keymatrix_report() {
  // Log pressed keys
  for (uint col = 0; col < N_COLS; ++col) {
    for (uint row = 0; row < N_ROWS; ++row) {
      // Check the pressed table and update
      if (pressed[col][row]) {
        printf("pressed %d %d\n", col + 1, row + 1);
      }
    }
  }
}

bool l84_keymatrix_is_key_pressed(uint col, uint row) {
  return pressed[col][row];
}

bool l84_keymatrix_is_fn_key_pressed() {
  return l84_keymatrix_is_key_pressed(L84_FN_COL, L84_FN_ROW);
}