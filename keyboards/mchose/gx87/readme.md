# GX87
**I TAKE NO RESPONSIBILITY FOR ANY DAMAGES CAUSED TO YOUR KEYBOARD BY USING THIS FIRMWARE. USE IT AT YOUR OWN RISK. MAKE SURE YOU HAVE BACKUPS OF THE STOCK FIRMWARE.**

[![GX87](https://s21.ax1x.com/2024/10/26/pAwjPw8.png)](https://imgse.com/i/pAwjPw8)

A customizable 80% multimodal keyboard.

* Keyboard Maintainer: [PugzAreCute](https://github.com/pugzarecute)
* Hardware Supported: MCHOSE GX87
* Hardware Availability: [MCHOSE GX87](https://www.mchose.store/products/mchose-gx87-aluminum-custom-mechanical-keyboard)

Make example for this keyboard (after setting up your build environment):

    make mchose/gx87:default

Flashing example for this keyboard:

    make mchose/gx87:default:flash
    
See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

This fork has almost completely reimpemented the stock firmware features missing in the provided "source". 


## Note
Unlike other keyboards based on WB QMK stack, wireless VIA will not be possible since the firmware for the dongle and onboard CH582D is "modified" and RAW seems broken. This keyboard is also unlike other keyboards since the dongle has the VID, PID, Manufacturer etc. hardcoded. On other keybaords, the dongle does not even ennumerate untill the KB has established a connection because it sends over the VID/PID but unfortunately this keyboard is not like that. The bluetooth names are also hardcoded onto the Onboard CH582D.

MCHOSE will not provide you with the stock firmware for this keyboard. Be sure to dump your own keyboard firmware before trying this firmware. **I TAKE NO RESPONSIBILITY FOR ANY DAMAGES CAUSED TO YOUR KEYBOARD BY USING THIS FIRMWARE. USE IT AT YOUR OWN RISK. MAKE SURE YOU HAVE BACKUPS OF THE STOCK FIRMWARE.**

You can dump the firmware by using SWD and the onboard unpopulated pin headers. Proceed by using OpenOCD and modify a STM32F1x target file to have the TAP ID of the WB32FQ95(`0x2ba01477`). The STM32F1x drivers can read the flash but I was unable to coerce it to write to it. We can use DFU mode to flash. Dumping via SWD will give you a nice clean dump.

You can also dump the firmware via DFU mode(NOTE: This is not the same as STM32 DFU mode and you need to use WB tools), but the format outputted by the WB32 DFU tool is not the same format it expects to flash, so be warned.

It should not be too hard to dump the onboard CH582D with WCHISP but I have not done so as it is not required.

In case you stop the programming mid-way through and reach a corrupted state where the application seems to start but does not work, locking you out of automated DFU and bootmagic, just short the BOOT0 pin to 3v3 by shorting the onboard unpopulated pin header.


## Bootloader

Enter the bootloader in 2 ways:

* **Bootmagic reset**: Hold down the Hold down the top left key (commonly programmed as *Esc*) and plug in the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
