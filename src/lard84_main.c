#include "tusb_config.h"
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/structs/io_bank0.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <pico/types.h>
#include <stdio.h> // printf via uart
#include <tusb.h>

const uint LED_PIN = 4;

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
    printf("intensity: %d, fade %d\n", led_intensity, fade_up);

    next_call_time = delayed_by_ms(now, 8);
  }
}

void core1_main() {
  /// Core 1 will poll the keymatrix and update the
  /// keyboard's state for core 0 to report via usb.

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

  // pwm_set_gpio_level(LED_PIN, 128);
  int led_intensity = 0;
  int fade_up = 1;

  while (true) {
    // gpio_put(LED_PIN, false);
    led_fade_task();
    // if (led_intensity >= 255) {
    // led_intensity = 0;
    // }
    // sleep_ms(500);
    // gpio_put(LED_PIN, true);
    // pwm_set_gpio_level(LED_PIN, 2);
    // sleep_ms(500);
    // printf("Hello, world!\n");

    // Every 1ms, poll the keymatrix
    // l84_keymatrix_poll();
  }
}

int main() {
  stdio_init_all();

  multicore_launch_core1(core1_main);

  tud_init(BOARD_TUD_RHPORT);

  while (true) {
    // sleep_ms(500);
    tud_task();
  }
}
