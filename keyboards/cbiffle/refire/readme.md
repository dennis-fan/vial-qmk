# reFIRE Rapid

Custom TKL keyboard based on QuickFire Rapid form factor.

* Keyboard Maintainer: [cbiffle](https://github.com/cbiffle)
* Hardware Supported: reFIRE Rapid rev A (STM32L412)
* Hardware Availability: Custom design - https://cliffle.com/widget/refire

## Features

- STM32L412 MCU
- NKRO support
- 87-key TKL layout (ANSI)
- WinLock functionality with LED indicator
- Hardware revision detection for safety
- EEPROM wear leveling with embedded flash

## WinLock Feature

Press Fn+F9 to toggle WinLock mode (Layer 2):
- Blocks GUI/Windows keys
- LED indicator in F9 key shows lock status
- Useful for gaming to prevent accidental Windows key presses

## Bootloader

Enter bootloader mode:
- **Bootmagic**: Hold ESC while plugging in
- **Keycode**: Press Fn+ESC (when Fn layer is active)
- **Hardware**: Board will auto-enter DFU if wrong firmware is flashed (revision protection)

## Hardware Revision Protection

The firmware includes automatic hardware revision detection:
- Tests GPIO pins A9 and A10 for floating state
- Forces bootloader entry if pins don't match expected configuration
- Prevents firmware mismatches that could damage the board

**Warning**: If you see the board immediately entering bootloader on power-up, you may have the wrong firmware version for your hardware revision.

## Building

Make example for this keyboard (after setting up your build environment):

    make cbiffle/refire:default
    make cbiffle/refire:vial

Flash using DFU:

    make cbiffle/refire:vial:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Vial Support

This keyboard supports [Vial](https://get.vial.today/), allowing you to remap keys and configure layers through a graphical interface without re-flashing firmware.

### Vial Unlock Combo

To unlock Vial for configuration changes:
- Press and hold **ESC + Backspace** simultaneously
- This security feature prevents unauthorized firmware modifications

### Layer Configuration

- Layer 0: Base QWERTY layout
- Layer 1: Function layer (Fn key held) - Media controls, volume, bootloader reset
- Layer 2: WinLock layer - GUI keys disabled
- Layer 3: Empty layer for custom configuration

All layers can be customized in the Vial configurator.
