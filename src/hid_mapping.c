/*******************************************************************************
 *   Password Manager application
 *   (c) 2017 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include "os.h"
#include "hid_mapping.h"

#define KEYCODE_START    0x20
#define MOD_MASK_LENGTH  12
#define MOD2_MASK_LENGTH 24
#define MAPPING_LENGTH   95

static const uint8_t TWOPOWER[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

static const uint8_t MAP_QWERTY[] = {
    // alt mask from ascii 0x20
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // shift mask from ascii 0x20
    0x7E,
    0x0f,
    0x00,
    0xd4,
    0xff,
    0xff,
    0xff,
    0xc7,
    0x00,
    0x00,
    0x00,
    0x78,
    // key codes from ascii 0x20
    0x2c,
    0x1e,
    0x34,
    0x20,
    0x21,
    0x22,
    0x24,
    0x34,
    0x26,
    0x27,
    0x25,
    0x2e,
    0x36,
    0x2d,
    0x37,
    0x38,
    0x27,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x33,
    0x33,
    0x36,
    0x2e,
    0x37,
    0x38,
    0x1f,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x0c,
    0x0d,
    0x0e,
    0x0f,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x2f,
    0x31,
    0x30,
    0x23,
    0x2d,
    0x35,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x0c,
    0x0d,
    0x0e,
    0x0f,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x2f,
    0x31,
    0x30,
    0x35};

static const uint8_t MAP_AZERTY[] = {
    // altgr mask from ascii 0x20
    0x08,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x78,
    0x01,
    0x00,
    0x00,
    0x78,
    // shift mask from ascii 0x20
    0x20,
    0xc8,
    0xff,
    0xc3,
    0xfe,
    0xff,
    0xff,
    0x07,
    0x00,
    0x00,
    0x00,
    0x00,
    // key codes from ascii 0x20
    0x2c,
    0x38,
    0x20,
    0x20,
    0x30,
    0x34,
    0x1e,
    0x21,
    0x22,
    0x2d,
    0x32,
    0x2e,
    0x10,
    0x23,
    0x36,
    0x37,
    0x27,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x37,
    0x36,
    0x64,
    0x2e,
    0x64,
    0x10,
    0x27,
    0x14,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x0c,
    0x0d,
    0x0e,
    0x0f,
    0x33,
    0x11,
    0x12,
    0x13,
    0x04,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1d,
    0x1b,
    0x1c,
    0x1a,
    0x22,
    0x25,
    0x2D,
    0x26,
    0x25,
    0x24,
    0x14,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x0c,
    0x0d,
    0x0e,
    0x0f,
    0x33,
    0x11,
    0x12,
    0x13,
    0x04,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1d,
    0x1b,
    0x1c,
    0x1a,
    0x21,
    0x23,
    0x2e,
    0x1f};

#if 0
// a good test string
out = "a&b~c#d  {e\"f'g(h  -i  _j)k=l+m  [n  |o  \\p^q  @r  ]s  }t$u!v:w/x;y.z,A?B  <C  >D`EFGHIJKLMNOPQRSTUVWXYZ0123456789";
#endif

void map_char(hid_mapping_t mapping, uint8_t key, uint8_t *out) {
    uint8_t keyDiv8, twoPower, keyCode, altUsed, shiftUsed;
    uint8_t *map;

    switch (mapping) {
        default:
            // THROW(EXCEPTION);
        case HID_MAPPING_QWERTY:
            map = (uint8_t *) MAP_QWERTY;
            break;
        case HID_MAPPING_AZERTY:
            map = (uint8_t *) MAP_AZERTY;
            break;
    }

    if (key < KEYCODE_START) {
        THROW(EXCEPTION);
    }
    key -= KEYCODE_START;
    if (key > MAPPING_LENGTH) {
        THROW(EXCEPTION);
    }
    keyDiv8 = (key / 8);
    twoPower = TWOPOWER[key % 8];
    altUsed = ((map[keyDiv8] & twoPower) != 0);
    shiftUsed = ((map[MOD_MASK_LENGTH + keyDiv8] & twoPower) != 0);
    keyCode = map[MOD2_MASK_LENGTH + key];
    out[0] = (altUsed ? 0x40 : 0x00) | (shiftUsed ? 0x02 : 0x00);
    out[1] = 0x00;
    out[2] = keyCode;
}
