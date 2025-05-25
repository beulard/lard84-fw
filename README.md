# lard84-fw

Firmware for the [lard84](https://github.com/beulard/lard84) keyboard, based on the RP2350 microcontroller.

## Build

First, clone the [pico-sdk](https://github.com/raspberrypi/pico-sdk.git) repo
and set your PICO_SDK_PATH:

```sh
export PICO_SDK_PATH=/path/to/pico-sdk
```

Then, build the project:

```sh
mkdir build
cd build
cmake ..
make -j
```

## Deploy

Drop the .uf2 file into the mass storage of the RP2350 stamp after rebooting it in bootsel mode.
