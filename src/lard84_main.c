/*
** file: lard84_keymatrix.h
** author: beulard (Matthias Dubouchet)
** creation date: 09/05/2025
*/

#include "class/hid/hid.h"
#include "class/hid/hid_device.h"
#include "lard84_keycodes.h"
#include "lard84_keymatrix.h"
#include "tusb_config.h"
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/structs/io_bank0.h>
#include <pico/multicore.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <pico/types.h>
#include <stdbool.h>
#include <stdio.h> // printf via uart
#include <tusb.h>

const uint LED_PIN = 4;

void led_fade_init() {
  gpio_init(LED_PIN);
  gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
  // gpio_set_dir(LED_PIN, GPIO_OUT);
  uint slice_num = pwm_gpio_to_slice_num(LED_PIN);
  pwm_config config = pwm_get_default_config();
  // Set divider, reduces counter clock to sysclock/this value
  pwm_config_set_clkdiv(&config, 4.f);
  // Load the configuration into our PWM slice, and set it running.
  pwm_init(slice_num, &config, true);
  pwm_set_wrap(slice_num, 512);
  // pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
  // pwm_set_chan_level(slice_num, PWM_CHAN_B, 3);
  pwm_set_enabled(slice_num, true);
}

void led_fade_task() {
  static absolute_time_t next_call_time = 0;
  absolute_time_t now = get_absolute_time();
  static int led_intensity = 0;
  static int fade_up = 1;

  int64_t delta_t = absolute_time_diff_us(now, next_call_time);

  if (delta_t < 0) {

    pwm_set_gpio_level(LED_PIN, led_intensity);
    led_intensity += 1 * fade_up;
    if (led_intensity > 64) {
      fade_up = -1;
    } else if (led_intensity <= 0) {
      fade_up = 1;
    }
    // printf("intensity: %d, fade %d\n", led_intensity, fade_up);

    next_call_time = delayed_by_ms(now, 8);
  }
}

void polling_task(mutex_t *mutex) {
  static absolute_time_t next_call_time = 0;
  absolute_time_t now = get_absolute_time();

  int64_t delta_t = absolute_time_diff_us(now, next_call_time);

  if (delta_t < 0) {
    absolute_time_t poll_start = get_absolute_time();

    l84_keymatrix_poll(mutex);

    absolute_time_t poll_done = get_absolute_time();
    int64_t poll_time_us = absolute_time_diff_us(poll_start, poll_done);

    if (poll_time_us > 1000) {
      next_call_time = poll_done;
    } else {
      next_call_time = delayed_by_us(poll_done, 1000 - poll_time_us);
    }
  }
}

void hid_task(mutex_t *mutex) {
  // Aim to send a HID report every 1ms
  static absolute_time_t next_call_time = 0;
  static absolute_time_t last_call_time = 0;
  absolute_time_t now = get_absolute_time();

  int64_t delta_t = absolute_time_diff_us(now, next_call_time);
  if (delta_t < 0) {
    // printf("Time to send HID\n");
    if (!tud_hid_ready()) {
      // printf("tud_hid_ready is FALSE\n");
      return;
    }
    last_call_time = get_absolute_time();

    uint8_t keycode[6] = {0};

    mutex_enter_blocking(mutex);
    uint keypressed_idx = 0;
    for (uint col = 0; col < N_COLS; ++col) {
      for (uint row = 0; row < N_ROWS; ++row) {
        if (l84_keymatrix_is_key_pressed(col, row)) {
          keycode[keypressed_idx++] =
              l84_keycode_get(col, row, l84_keymatrix_is_fn_key_pressed());
          if (keypressed_idx > 5) {
            break;
          }
        }
      }
    }
    mutex_exit(mutex);

    tud_hid_keyboard_report(0, 0, keycode);

    next_call_time = delayed_by_ms(get_absolute_time(), 1);
  }
}

static mutex_t keymatrix_mutex;

void core1_main() {
  /// Core 1 will poll the keymatrix and update the
  /// keyboard's state for core 0 to report via usb.

  led_fade_init();

  const uint window_size = 256;
  int64_t loop_time_window[window_size];
  uint16_t cycle_idx = 0;

  absolute_time_t last_print = 0;

  while (true) {
    absolute_time_t loop_start = get_absolute_time();

    led_fade_task();
    polling_task(&keymatrix_mutex);

    absolute_time_t loop_done = get_absolute_time();

    int64_t loop_time_us = absolute_time_diff_us(loop_start, loop_done);

    loop_time_window[cycle_idx % window_size] = loop_time_us;
    double avg = 0.0;
    for (uint i = 0; i < window_size; ++i) {
      avg += (double)(loop_time_window[i]) / window_size;
    }
    cycle_idx++;

    if (absolute_time_diff_us(last_print, loop_done) > 500000) {
      printf("Core1 loop time: %lldus (avg %lldus)\n", loop_time_us,
             (int64_t)avg);
      last_print = loop_done;
    }
  }
}

int main() {
  stdio_init_all();

  mutex_init(&keymatrix_mutex);
  l84_keymatrix_setup();

  multicore_launch_core1(core1_main);

  tud_init(BOARD_TUD_RHPORT);

  const uint window_size = 256;
  int64_t loop_time_window[window_size];
  uint16_t cycle_idx = 0;

  int64_t max_loop_time = 0;

  absolute_time_t last_print = 0;

  while (true) {
    absolute_time_t loop_start = get_absolute_time();

    tud_task();

    hid_task(&keymatrix_mutex);

    absolute_time_t loop_done = get_absolute_time();
    int64_t loop_time_us = absolute_time_diff_us(loop_start, loop_done);

    loop_time_window[cycle_idx % window_size] = loop_time_us;
    double avg = 0.0;
    for (uint i = 0; i < window_size; ++i) {
      avg += (double)(loop_time_window[i]) / window_size;
    }
    cycle_idx++;

    if (absolute_time_diff_us(last_print, loop_done) > 500000) {
      printf("Core0 loop time: %lldus (avg %lldus)\n", loop_time_us,
             (int64_t)avg);
      last_print = loop_done;
    }
  }
}
