"""Convert a Vial .vil save into a QMK keymap.c plus Vial EEPROM defaults.

usage: vil2keymap.py latest.vil info.json out_dir
"""
import json, re, sys

vil_path, info_path, out_dir = sys.argv[1:4]
vil = json.load(open(vil_path))
info = json.load(open(info_path))

RENAME = {
    "KC_GRAVE": "KC_GRV", "KC_ESCAPE": "KC_ESC", "KC_LSHIFT": "KC_LSFT", "KC_RSHIFT": "KC_RSFT",
    "KC_EQUAL": "KC_EQL", "KC_BSPACE": "KC_BSPC", "KC_SCOLON": "KC_SCLN", "KC_LBRACKET": "KC_LBRC",
    "KC_RBRACKET": "KC_RBRC", "KC_BSLASH": "KC_BSLS", "KC_QUOTE": "KC_QUOT", "KC_COMMA": "KC_COMM",
    "KC_SLASH": "KC_SLSH", "KC_MINUS": "KC_MINS", "KC_PGDOWN": "KC_PGDN", "KC_DELETE": "KC_DEL",
    "KC_PSCREEN": "KC_PSCR", "KC_SCROLLLOCK": "KC_SCRL", "KC_PAUSE": "KC_PAUS", "KC_CAPSLOCK": "KC_CAPS",
    "KC_NUMLOCK": "KC_NUM", "KC_NONUS_BSLASH": "KC_NUBS", "KC_ENTER": "KC_ENT", "KC_SPACE": "KC_SPC",
    "KC_KP_0": "KC_P0", "KC_KP_1": "KC_P1", "KC_KP_2": "KC_P2", "KC_KP_3": "KC_P3", "KC_KP_4": "KC_P4",
    "KC_KP_5": "KC_P5", "KC_KP_6": "KC_P6", "KC_KP_7": "KC_P7", "KC_KP_8": "KC_P8", "KC_KP_9": "KC_P9",
    "KC_KP_DOT": "KC_PDOT", "KC_KP_ENTER": "KC_PENT", "KC_KP_EQUAL": "KC_PEQL", "KC_KP_SLASH": "KC_PSLS",
    "KC_KP_ASTERISK": "KC_PAST", "KC_KP_MINUS": "KC_PMNS", "KC_KP_PLUS": "KC_PPLS",
    "KC_LCTRL": "KC_LCTL", "KC_RCTRL": "KC_RCTL", "KC_LGUI": "KC_LGUI", "KC_RGUI": "KC_RGUI",
    "KC_LALT": "KC_LALT", "KC_RALT": "KC_RALT", "KC_HOME": "KC_HOME", "KC_INSERT": "KC_INS",
    "KC_RIGHT": "KC_RGHT", "KC_DOT": "KC_DOT",
}
SPECIAL = {
    "QK_CAPS_WORD_TOGGLE": "CW_TOGG", "AU_TOG": "AU_TOGG", "HPT_TOG": "HF_TOGG",
    "QK_REBOOT": "QK_RBT", "KC_TRNS": "_______",
}

def norm(kc):
    if kc in (-1, None):
        return "KC_NO"
    kc = str(kc)
    if kc in SPECIAL:
        return SPECIAL[kc]
    m = re.fullmatch(r"M(\d+)", kc)
    if m:
        return f"MC_{m.group(1)}"
    return re.sub(r"KC_[A-Z0-9_]+", lambda m: RENAME.get(m.group(0), m.group(0)), kc)

layout = info["layouts"]["LAYOUT"]["layout"]
rows_used = sum(1 for l in vil["layout"] if any(k not in ("KC_NO", "KC_TRNS", -1) for r in l for k in r))

# ---- keymap.c -------------------------------------------------------------
out = []
out.append("// Generated from latest.vil by vil2keymap.py -- edit the .vil in Vial, then regenerate.\n")
out.append("#include QMK_KEYBOARD_H\n")
out.append('#include "vial_defaults.h"\n\n')
out.append("const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {\n")
for li, layer in enumerate(vil["layout"]):
    out.append(f"    [{li}] = LAYOUT(\n")
    cur_y = None
    line = []
    lines = []
    for key in layout:
        r, c = key["matrix"]
        if cur_y is not None and key["y"] != cur_y:
            lines.append(line); line = []
        cur_y = key["y"]
        line.append(norm(layer[r][c]))
    lines.append(line)
    body = []
    for i, l in enumerate(lines):
        sep = "," if i < len(lines) - 1 else ""
        body.append("        " + ", ".join(f"{k:<16}" for k in l).rstrip() + sep)
    out.append("\n".join(body) + "\n")
    out.append("    )" + ("," if li < len(vil["layout"]) - 1 else "") + "\n")
out.append("};\n")
open(f"{out_dir}/keymap.c", "w").write("".join(out))

# ---- vial_defaults.h ------------------------------------------------------
SS_QMK_PREFIX, SS_TAP, SS_DOWN, SS_UP, SS_DELAY = 1, 1, 2, 3, 4
EXT_TAP, EXT_DOWN, EXT_UP = 5, 6, 7

def macro_bytes(macro):
    b = bytearray()
    for act in macro:
        kind = act[0]
        if kind == "text":
            b += act[1].encode("utf-8")
        elif kind == "delay":
            ms = int(act[1])
            b += bytes([SS_QMK_PREFIX, SS_DELAY, ms % 255 + 1, ms // 255 + 1])
        elif kind in ("tap", "down", "up"):
            tag = {"tap": SS_TAP, "down": SS_DOWN, "up": SS_UP}[kind]
            for kc in act[1:]:
                b += bytes([SS_QMK_PREFIX, tag]) + (f"__KC__{norm(kc)}__").encode()
        else:
            raise SystemExit(f"unknown macro action {act}")
    return bytes(b)

h = []
h.append("// Generated from latest.vil by vil2keymap.py -- Vial EEPROM defaults.\n#pragma once\n\n")
h.append("#include \"vial.h\"\n\n")
tds = [t for t in vil["tap_dance"] if any(x not in ("KC_NO", 0) for x in t[:4])]
h.append("static const vial_tap_dance_entry_t PROGMEM vial_default_tap_dances[] = {\n")
for i, t in enumerate(vil["tap_dance"][:len(tds)]):
    h.append(f"    /* TD({i}) */ {{ {norm(t[0])}, {norm(t[1])}, {norm(t[2])}, {norm(t[3])}, {t[4]} }},\n")
h.append("};\n\n")
combos = [(i, c) for i, c in enumerate(vil["combo"]) if sum(1 for x in c[:4] if x != "KC_NO") >= 2]
h.append("static const vial_combo_entry_t PROGMEM vial_default_combos[] = {\n")
for i, c in combos:
    h.append(f"    /* combo {i} */ {{ {{ {norm(c[0])}, {norm(c[1])}, {norm(c[2])}, {norm(c[3])} }}, {norm(c[4])} }},\n")
h.append("};\n\n")
# macro buffer: emit as a C string-ish byte array with keycodes as symbols
macros = vil["macro"]
while macros and not macros[-1]:
    macros.pop()
h.append("static const uint8_t PROGMEM vial_default_macro_buffer[] = {\n")
for i, m in enumerate(macros):
    desc = " ".join(" ".join(map(str, a)) for a in m)
    h.append(f"    /* M{i}: {desc} */\n    ")
    raw = macro_bytes(m)
    parts = []
    s = raw.decode("latin-1")
    pos = 0
    for mm in re.finditer(r"__KC__(.+?)__", s):
        for ch in s[pos:mm.start()]:
            parts.append(f"0x{ord(ch):02x}")
        kc = mm.group(1)
        parts.append(f"VIAL_MACRO_KC({kc})")
        pos = mm.end()
    for ch in s[pos:]:
        parts.append(f"0x{ord(ch):02x}")
    parts.append("0x00")
    h.append(", ".join(parts) + ",\n")
h.append("};\n")
open(f"{out_dir}/vial_defaults.h", "w").write("".join(h))
print("layers:", len(vil["layout"]), "tap dances:", len(tds), "combos:", len(combos), "macros:", len(macros))
