# Porting QMK Keyboards to Vial: Complete Guide

This guide documents the complete process for porting an existing QMK keyboard to Vial, including all steps, common issues, and lessons learned.

## Prerequisites

Before starting, ensure you have:
- A working QMK keyboard implementation in another repository
- QMK CLI installed (`pip install qmk`)
- Git submodules initialized in vial-qmk (`git submodule update --init --recursive`)
- ARM/AVR toolchains installed (via `curl -fsSL https://install.qmk.fm | sh`)
- Python 3 for automation scripts

## Overview

**Time estimate**: 2-3 hours for a straightforward port
**Difficulty**: Intermediate (requires understanding of JSON, C, and keyboard matrices)

### What Gets Ported
- Hardware definition files (keyboard.json, config.h, MCU configs)
- Core keyboard code (keyboard.c with custom initialization)
- Default keymap as reference
- All special features (LEDs, custom layers, hardware checks)

### What Gets Created
- Vial keymap directory with 4 files:
  - `config.h` - Vial UID and security
  - `rules.mk` - Build configuration
  - `keymap.c` - Layer definitions
  - `vial.json` - Visual layout for Vial GUI

## Step-by-Step Process

### Phase 1: Repository Setup

#### 1.1 Create Working Branch
```bash
cd /path/to/vial-qmk
git checkout vial
git pull upstream vial
git checkout -b <keyboard-name>
```

**Why**: Isolates your work and allows easy PR submission or rollback.

#### 1.2 Create Directory Structure
```bash
mkdir -p keyboards/<vendor>/<keyboard>/keymaps/default
mkdir -p keyboards/<vendor>/<keyboard>/keymaps/vial
```

**Standard structure**:
```
keyboards/<vendor>/<keyboard>/
├── keyboard.json          # Hardware definition
├── config.h               # Board-level config
├── rules.mk               # Build rules
├── <keyboard>.c           # Hardware initialization
├── halconf.h              # HAL config (STM32 only)
├── mcuconf.h              # MCU config (STM32 only)
├── readme.md              # Documentation
└── keymaps/
    ├── default/
    │   └── keymap.c       # Original keymap
    └── vial/
        ├── keymap.c       # Vial-compatible keymap
        ├── vial.json      # Layout definition
        ├── config.h       # Vial configuration
        └── rules.mk       # Vial build flags
```

### Phase 2: Copy Core Files

#### 2.1 Copy Hardware Definition Files

**Essential files to copy from source QMK repo**:
```bash
cp source/keyboards/<path>/keyboard.json target/keyboards/<path>/
cp source/keyboards/<path>/config.h target/keyboards/<path>/
cp source/keyboards/<path>/rules.mk target/keyboards/<path>/
cp source/keyboards/<path>/<keyboard>.c target/keyboards/<path>/
```

**For STM32 keyboards, also copy**:
```bash
cp source/keyboards/<path>/halconf.h target/keyboards/<path>/
cp source/keyboards/<path>/mcuconf.h target/keyboards/<path>/
```

**For AVR keyboards, also copy**:
```bash
cp source/keyboards/<path>/<keyboard>.h target/keyboards/<path>/
```

#### 2.2 Copy Default Keymap
```bash
cp source/keyboards/<path>/keymaps/default/keymap.c \
   target/keyboards/<path>/keymaps/default/
```

**Why**: Preserves original layout as reference and ensures default keymap compiles.

#### 2.3 Fix JSON Syntax Issues

**Common issue**: QMK keyboard.json files may have trailing commas (invalid JSON).

**Find and fix**:
```bash
# Look for these patterns:
],      # Trailing comma in array
},      # Trailing comma in object
```

**Example fixes**:
```json
// BEFORE (invalid)
"matrix_pins": {
    "cols": ["B0", "B13", "B14"],
    "rows": ["A0", "A2", "A1"],  ← trailing comma
},

// AFTER (valid)
"matrix_pins": {
    "cols": ["B0", "B13", "B14"],
    "rows": ["A0", "A2", "A1"]   ← no trailing comma
},
```

### Phase 3: Create Vial Configuration

#### 3.1 Generate Unique Keyboard UID

**Generate random 8-byte UID**:
```bash
python3 -c "import random; print('{' + ', '.join([f'0x{random.randint(0,255):02X}' for _ in range(8)]) + '}')"
```

**Output example**: `{0xAE, 0x31, 0xC7, 0xE5, 0x00, 0x6F, 0x1F, 0x1D}`

#### 3.2 Create `keymaps/vial/config.h`

**Template**:
```c
/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

// Generated UID - paste output from step 3.1
#define VIAL_KEYBOARD_UID {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}

// Unlock combo: two keys that must be pressed together to unlock Vial
// Common choices: ESC + Backspace, ESC + Enter, or opposite corners
#define VIAL_UNLOCK_COMBO_ROWS { <row1>, <row2> }
#define VIAL_UNLOCK_COMBO_COLS { <col1>, <col2> }
```

**How to choose unlock combo**:
1. Look at `keyboard.json` → `layouts` → `layout` array
2. Find two easily accessible keys (ESC and Backspace are common)
3. Note their `matrix` values: `[row, col]`
4. Use those row/col values in the defines

**Example for ESC[5,1] + Backspace[3,9]**:
```c
#define VIAL_UNLOCK_COMBO_ROWS { 5, 3 }
#define VIAL_UNLOCK_COMBO_COLS { 1, 9 }
```

#### 3.3 Create `keymaps/vial/rules.mk`

**Standard template**:
```makefile
VIA_ENABLE = yes
VIAL_ENABLE = yes
LTO_ENABLE = yes

# Disable features to save space
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
QMK_SETTINGS = no
COMBO_ENABLE = no
KEY_OVERRIDE_ENABLE = no
```

**Size optimization options** (add if firmware is too large):
```makefile
MOUSEKEY_ENABLE = no
SPACE_CADET_ENABLE = no
MAGIC_ENABLE = no
GRAVE_ESC_ENABLE = no
TAP_DANCE_ENABLE = no
```

**Why LTO_ENABLE**: Link-Time Optimization reduces firmware size by 10-20%.

#### 3.4 Create `keymaps/vial/keymap.c`

**Process**:
1. Copy from `keymaps/default/keymap.c`
2. Verify layer count matches `keyboard.json` → `dynamic_keymap` → `layer_count`
3. Add empty layers if needed to reach the count
4. **PRESERVE all custom functions** from default keymap

**Template structure**:
```c
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_xxx(
        // Base layer keys
    ),
    [1] = LAYOUT_xxx(
        // Fn layer or similar
    ),
    // Add layers 2, 3, etc. as needed
    [N] = LAYOUT_xxx(
        // All keys as _______ for user customization
    ),
};

// CRITICAL: Preserve all custom functions from default keymap
// Examples:
// - keyboard_pre_init_user()
// - keyboard_post_init_user()
// - layer_state_set_user()
// - encoder_update_user()
// - matrix_scan_user()
```

**Common custom functions to preserve**:
- **LED initialization**: `keyboard_post_init_user()` or `keyboard_pre_init_user()`
- **Layer indicators**: `layer_state_set_user(state)`
- **Rotary encoders**: `encoder_update_user(index, clockwise)`
- **RGB effects**: `rgb_matrix_indicators_user()` or similar
- **OLED displays**: `oled_task_user()`

**Example of preserved custom code**:
```c
// From refire keyboard - WinLock LED control
void keyboard_pre_init_user(void) {
    gpio_write_pin_high(A8);  // Initialize WinLock LED
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Control LED based on layer 2 (WinLock) state
    if (IS_LAYER_ON_STATE(state, 2)) {
        gpio_write_pin_low(A8);
    } else {
        gpio_write_pin_high(A8);
    }
    return state;
}
```

#### 3.5 Create `keymaps/vial/vial.json`

This is the most complex file. It defines the visual layout for the Vial configurator.

**Automated generation script**:
```python
import json

# Read source keyboard.json
with open('keyboards/<vendor>/<keyboard>/keyboard.json') as f:
    kbd = json.load(f)

# Get the layout (adjust LAYOUT_xxx to match your keyboard)
layout = kbd['layouts']['LAYOUT_xxx']['layout']

# Convert to Vial format
keymap = []
current_row = []
last_y = 0.0

for key in layout:
    # Start new row if y coordinate increases
    if key.get('y', last_y) > last_y:
        if current_row:
            keymap.append(current_row)
        current_row = []
        last_y = key.get('y', last_y)

    # Add position info (x, y, w for wide keys)
    entry = {}
    if 'x' in key or 'y' in key or 'w' in key:
        entry = {k: v for k, v in key.items() if k in ['x', 'y', 'w', 'h']}
        current_row.append(entry)

    # Add matrix position as "row,col"
    matrix_str = f"{key['matrix'][0]},{key['matrix'][1]}"
    current_row.append(matrix_str)

if current_row:
    keymap.append(current_row)

# Get USB IDs from keyboard.json
usb = kbd['usb']

# Create vial.json
vial_json = {
    "name": kbd.get('keyboard_name', 'My Keyboard'),
    "vendorId": usb['vid'],
    "productId": usb['pid'],
    "lighting": "none",  # Change if keyboard has RGB
    "matrix": {
        "rows": kbd['matrix_pins']['rows'].__len__(),
        "cols": kbd['matrix_pins']['cols'].__len__()
    },
    "layouts": {
        "keymap": keymap
    }
}

# Write vial.json
with open('keyboards/<vendor>/<keyboard>/keymaps/vial/vial.json', 'w') as f:
    json.dump(vial_json, f, indent=2)

print("vial.json generated successfully")
```

**Lighting options**:
- `"none"` - No RGB, or only simple indicator LEDs
- `"qmk_rgblight"` - QMK RGB underglow
- `"qmk_backlight"` - QMK backlight
- `"vialrgb"` - Vial RGB matrix

**For keyboards with layout options**, add `labels` array:
```json
{
  "layouts": {
    "labels": [
      "Split Backspace",
      "ISO Enter",
      ["Bottom Row", "ANSI", "ISO", "HHKB"]
    ],
    "keymap": [ ... ]
  }
}
```

### Phase 4: Documentation

#### 4.1 Create `readme.md`

**Template**:
```markdown
# [Keyboard Name]

[Brief description]

* Keyboard Maintainer: [GitHub username](https://github.com/username)
* Hardware Supported: [Hardware details, MCU]
* Hardware Availability: [Link to purchase or "Custom design"]

## Features

- [List key features]
- [MCU type]
- [Special features]

## Bootloader

Enter bootloader mode:
- **Bootmagic**: Hold [key] while plugging in
- **Keycode**: Press [Fn+key combination]
- **Physical**: [Hardware reset method if applicable]

## Building

Make example for this keyboard:

    make <vendor>/<keyboard>:default
    make <vendor>/<keyboard>:vial

Flash using:

    make <vendor>/<keyboard>:vial:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.

## Vial Support

This keyboard supports [Vial](https://get.vial.today/).

### Vial Unlock Combo

To unlock Vial: Press and hold **[KEY1] + [KEY2]** simultaneously.

### Layers

- Layer 0: [Description]
- Layer 1: [Description]
- Layer N: Empty layer for custom configuration
```

### Phase 5: Build and Test

#### 5.1 Compile Default Keymap

```bash
cd /path/to/vial-qmk
export PATH="/Users/$USER/.local/bin:$PATH"  # Or your qmk path
qmk compile -kb <vendor>/<keyboard> -km default
```

**Expected output**: `.bin` or `.hex` file in `.build/` directory

**Common errors and solutions**:

**Error**: `No such file or directory: lib/chibios-contrib/...`
```bash
# Solution: Initialize git submodules
git submodule update --init --recursive
```

**Error**: `No rule to make target`
```bash
# Solution: Check that rules.mk, halconf.h, mcuconf.h are present
# For STM32, ensure halconf.h and mcuconf.h exist
```

**Error**: `error: 'XXX' undeclared`
```bash
# Solution: Missing include or feature flag in rules.mk
# Check source repo's rules.mk for required features
```

#### 5.2 Compile Vial Keymap

```bash
qmk compile -kb <vendor>/<keyboard> -km vial
```

#### 5.3 Check Firmware Size

```bash
arm-none-eabi-size .build/<keyboard>_vial.elf
# or for AVR:
avr-size .build/<keyboard>_vial.hex
```

**Flash size limits**:
- ATmega32U4: 28KB (28,672 bytes)
- STM32F072: 128KB (131,072 bytes)
- STM32F103: 64KB/128KB depending on variant
- STM32L412: 128KB (131,072 bytes)
- RP2040: 2MB (plenty of space)

**If firmware is too large**:
1. Add more disabled features to `vial/rules.mk` (see section 3.3)
2. Remove unused layers from keymap.c
3. Consider disabling VIA if only Vial is needed: `VIA_ENABLE = no`
4. Last resort: Use VIAL_INSECURE (not recommended, see Security below)

### Phase 6: Commit Changes

#### 6.1 Stage Files
```bash
git add keyboards/<vendor>/<keyboard>
git status  # Verify correct files staged
```

#### 6.2 Create Commit

**Good commit message template**:
```
Add <vendor>/<keyboard> keyboard with Vial support

- [Brief description of keyboard]
- [Key features]
- [Hardware specs]

Features:
- [List important features]
- [Custom functionality preserved]
- [LED indicators, encoders, etc.]

Vial Configuration:
- Unlock combo: [KEY1] + [KEY2]
- UID: {0xXX, 0xXX, ...}
- Default keymap: [size] bytes
- Vial keymap: [size] bytes
```

**Example**:
```bash
git commit -m "Add cbiffle/refire keyboard with Vial support

- Custom TKL keyboard with STM32L412 MCU
- 87-key ANSI layout
- Hardware revision detection in firmware
- WinLock layer with LED indicator (Fn+F9 toggle)

Features:
- NKRO support
- 3 LED indicators: Caps Lock, Scroll Lock, WinLock
- Open-drain LED configuration
- Hardware revision check prevents firmware mismatches

Vial Configuration:
- Unlock combo: ESC + Backspace
- UID: {0xAE, 0x31, 0xC7, 0xE5, 0x00, 0x6F, 0x1F, 0x1D}
- Default keymap: 26,188 bytes
- Vial keymap: 30,704 bytes"
```

## Common Issues and Solutions

### Issue: Keyboard.json has trailing commas

**Symptom**: Python JSON parsing error
```
json.decoder.JSONDecodeError: Expecting property name enclosed in double quotes
```

**Solution**: Remove trailing commas from keyboard.json
```json
// Find patterns like:
],  ← Remove this comma
}   ← This one is OK (not last in object)

},  ← Remove this comma if last in parent object
```

### Issue: Missing custom hardware initialization

**Symptom**: LEDs don't work, encoders don't respond, features broken

**Solution**: Check source keyboard's `.c` file for these functions:
- `keyboard_pre_init_kb()` / `keyboard_pre_init_user()`
- `keyboard_post_init_kb()` / `keyboard_post_init_user()`
- `matrix_init_user()` / `matrix_scan_user()`

Copy ALL custom functions to `vial/keymap.c` or ensure they're in keyboard.c.

**Example - Preserving encoder code**:
```c
// In vial/keymap.c
#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return false;
}
#endif
```

### Issue: Firmware too large

**Symptom**: Linker error about exceeding region size
```
region 'rom' overflowed by XXX bytes
```

**Solutions** (in order of preference):
1. **Enable LTO**: Already in template rules.mk
2. **Disable unused features**: Add to vial/rules.mk:
   ```makefile
   MOUSEKEY_ENABLE = no
   SPACE_CADET_ENABLE = no
   GRAVE_ESC_ENABLE = no
   MAGIC_ENABLE = no
   TAP_DANCE_ENABLE = no
   ```
3. **Reduce layers**: Remove unused empty layers from keymap.c
4. **Use VIAL_INSECURE** (last resort, not recommended):
   ```c
   // In vial/config.h
   #define VIAL_INSECURE
   ```
   ⚠️ This disables security features and the keyboard won't be accepted into official Vial repo.

### Issue: Matrix positions don't match in Vial GUI

**Symptom**: Keys appear in wrong locations in Vial configurator

**Solution**: Verify vial.json matrix positions match keyboard.json:
1. Open keyboard.json and find a key's position
2. Note its `matrix: [row, col]` value
3. In vial.json, find the corresponding `"row,col"` string
4. Verify the x,y coordinates match the physical position

**Debug script**:
```python
import json

with open('keyboard.json') as f:
    kbd = json.load(f)
with open('keymaps/vial/vial.json') as f:
    vial = json.load(f)

# Check matrix dimensions match
print(f"keyboard.json: {len(kbd['matrix_pins']['rows'])}x{len(kbd['matrix_pins']['cols'])}")
print(f"vial.json: {vial['matrix']['rows']}x{vial['matrix']['cols']}")

# Check all matrix positions exist
layout = kbd['layouts'][list(kbd['layouts'].keys())[0]]['layout']
for key in layout:
    row, col = key['matrix']
    search = f"{row},{col}"
    found = any(search in str(row) for row in vial['layouts']['keymap'])
    if not found:
        print(f"Missing: {search} (Label: {key.get('label', 'N/A')})")
```

### Issue: Vial doesn't recognize keyboard

**Symptoms**:
- Vial shows "No device found"
- Keyboard works normally but Vial can't see it
- Other keyboards show up but not this one

**Solutions**:
1. **Check VIA_ENABLE and VIAL_ENABLE** in vial/rules.mk:
   ```makefile
   VIA_ENABLE = yes    # Required
   VIAL_ENABLE = yes   # Required
   ```

2. **Verify vial.json is valid JSON**:
   ```bash
   python3 -m json.tool keymaps/vial/vial.json
   ```

3. **Check USB VID/PID match** between keyboard.json and vial.json:
   ```bash
   # keyboard.json
   "usb": {
     "vid": "0x4705",
     "pid": "0xF14E"
   }

   # vial.json must match
   "vendorId": "0x4705",
   "productId": "0xF14E"
   ```

4. **Rebuild and reflash**: Sometimes cache issues require clean rebuild:
   ```bash
   qmk clean -a
   qmk compile -kb <keyboard> -km vial
   ```

## Security Considerations

### Unlock Combos

**Purpose**: Prevents unauthorized firmware modifications by requiring two keys to be pressed simultaneously to unlock Vial.

**Best practices**:
- Choose keys that exist in ALL layout variants (if keyboard has multiple layouts)
- Avoid common typing combinations (don't use A+S or similar)
- Document clearly in readme.md
- Common choices:
  - ESC + Backspace (opposite corners of main block)
  - ESC + Enter (far apart)
  - Top-left + Bottom-right keys

**Bad unlock combo example**:
```c
// BAD: These might not exist in all layouts
#define VIAL_UNLOCK_COMBO_ROWS { 0, 0 }  // Same row
#define VIAL_UNLOCK_COMBO_COLS { 0, 1 }  // Adjacent keys
```

**Good unlock combo example**:
```c
// GOOD: Opposite corners, always present
#define VIAL_UNLOCK_COMBO_ROWS { 0, 4 }  // Top and bottom rows
#define VIAL_UNLOCK_COMBO_COLS { 0, 13 } // Left and right sides
```

### VIAL_INSECURE

**Never use unless absolutely necessary.**

```c
#define VIAL_INSECURE  // Disables all security features
```

**When it's acceptable**:
- Prototyping during development
- Personal builds you'll never share
- Firmware size is critical AND you trust all users

**When it's NOT acceptable**:
- Submitting to official Vial repository
- Distributing to others
- Any public keyboard

**Consequences**:
- Anyone with physical access can modify firmware settings
- No protection against malicious configuration
- Pull requests with VIAL_INSECURE will be rejected

## Advanced Topics

### Multiple Layout Support

For keyboards with multiple physical layouts (ISO/ANSI, split backspace, etc.):

**In vial.json**:
```json
{
  "layouts": {
    "labels": [
      "Split Backspace",
      "ISO Enter",
      ["Bottom Row", "ANSI", "ISO", "HHKB"]
    ],
    "keymap": [
      // First key in each position = default option
      // Add multiple keys at same position for layout variants
      [
        "0,0",           // ESC (always present)
        "0,1",           // Key that's always there
        // ...
        "2,13\n\n\n0,0", // Split backspace option 0, key 0
        "2,13\n\n\n0,1", // Split backspace option 0, key 1
        "2,13\n\n\n1,0", // Split backspace option 1 (unified)
      ]
    ]
  }
}
```

**Label syntax**: `"row,col\n\n\noption_index,key_index"`
- First `\n\n\n` is separator
- `option_index` = which layout option (0-based)
- `key_index` = which key within that option (0-based)

### RGB Lighting

**For RGB underglow/backlight**:

In vial.json:
```json
{
  "lighting": "qmk_rgblight",
  "menus": ["qmk_rgblight"]
}
```

In vial/rules.mk:
```makefile
RGBLIGHT_ENABLE = yes
```

**For RGB Matrix**:

In vial.json:
```json
{
  "lighting": "qmk_rgb_matrix",
  "menus": ["qmk_rgb_matrix"]
}
```

In vial/rules.mk:
```makefile
RGB_MATRIX_ENABLE = yes
```

### Rotary Encoders

Encoders work automatically with Vial if defined in keyboard.json:

```json
{
  "encoder": {
    "enabled": true,
    "rotary": [
      {"pin_a": "B12", "pin_b": "B13", "resolution": 4}
    ]
  }
}
```

**In vial/keymap.c**, define default behavior:
```c
#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return false;
}
#endif
```

Users can override this in Vial GUI.

## Validation Checklist

Before submitting or considering the port complete:

### Files Present
- [ ] keyboard.json (valid JSON, no trailing commas)
- [ ] config.h
- [ ] rules.mk
- [ ] <keyboard>.c
- [ ] keymaps/default/keymap.c
- [ ] keymaps/vial/keymap.c
- [ ] keymaps/vial/config.h (with UID and unlock combo)
- [ ] keymaps/vial/rules.mk (VIA_ENABLE=yes, VIAL_ENABLE=yes)
- [ ] keymaps/vial/vial.json (valid JSON)
- [ ] readme.md
- [ ] halconf.h and mcuconf.h (STM32 only)

### Build Tests
- [ ] Default keymap compiles without errors
- [ ] Vial keymap compiles without errors
- [ ] Firmware size is within MCU flash limit
- [ ] No warnings during compilation

### Configuration Validation
- [ ] vial.json matrix dimensions match keyboard.json
- [ ] vial.json USB VID/PID match keyboard.json
- [ ] Unlock combo uses valid matrix positions
- [ ] Unlock combo is documented in readme.md
- [ ] VIAL_KEYBOARD_UID is unique (randomly generated)
- [ ] All layers from keyboard.json are present in keymap.c

### Feature Preservation
- [ ] All custom initialization functions copied from source
- [ ] LED indicators work (if applicable)
- [ ] Encoders work (if applicable)
- [ ] Special layer functionality preserved
- [ ] Hardware checks/safeguards preserved

### Documentation
- [ ] readme.md describes keyboard features
- [ ] Bootloader entry methods documented
- [ ] Vial unlock combo documented
- [ ] Special features explained (WinLock, custom layers, etc.)
- [ ] Build commands included

### Vial Testing (if hardware available)
- [ ] Vial recognizes keyboard
- [ ] All keys appear in correct positions
- [ ] Keys can be remapped
- [ ] Layer switching works
- [ ] Settings persist across power cycles
- [ ] Unlock combo works correctly

## Reference: refire Keyboard Port

The refire keyboard port (commit 6cf5eb864e) demonstrates a complete implementation:

**Preserved custom features**:
- Hardware revision detection (`keyboard_pre_init_kb`)
- Open-drain LED configuration (`keyboard_post_init_user`)
- WinLock layer with LED indicator (`layer_state_set_user`)

**Files created**:
- 12 files total
- 943 lines of code
- Default keymap: 26,188 bytes
- Vial keymap: 30,704 bytes (within 128KB limit)

**Key decisions**:
- Unlock combo: ESC[5,1] + Backspace[3,9] (opposite corners)
- 4 layers: Base QWERTY, Fn overlay, WinLock, Custom
- Preserved all 3 LED indicators (Caps, Scroll, WinLock)
- Fixed trailing commas in keyboard.json for valid JSON

See commit `6cf5eb864e` for complete implementation reference.

## Getting Help

**Resources**:
- Vial Documentation: https://get.vial.today/docs/
- QMK Documentation: https://docs.qmk.fm/
- Vial Discord: https://discord.gg/zNKEUXTKwF
- QMK Discord: https://discord.gg/Uq7gcHh

**When asking for help, provide**:
1. Link to source keyboard in QMK repo
2. MCU type and flash size
3. Compilation errors (full output)
4. What you've already tried
5. Keyboard.json and vial.json (if relevant)

## Conclusion

Porting a keyboard to Vial typically takes 2-3 hours for straightforward keyboards. Complex keyboards with multiple layouts, RGB, encoders, and custom features may take longer.

**Success factors**:
- Careful preservation of custom hardware code
- Valid JSON syntax (no trailing commas)
- Proper matrix position mapping
- Adequate firmware size optimization
- Complete documentation

**Most common pitfalls**:
- Forgetting to copy custom initialization functions
- Invalid JSON syntax in keyboard.json or vial.json
- Mismatched matrix positions between files
- Insufficient firmware size optimization
- Missing git submodules

Follow this guide systematically and most ports should be straightforward. When in doubt, refer to existing Vial keyboards in the repository for examples.
