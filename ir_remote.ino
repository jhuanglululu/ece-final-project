#ifndef IR_REMOTE_INO
#define IR_REMOTE_INO

/* this file includes ir remote logic */

#include "joystick.ino"

void init_ir_remote();
joystick_dir_t get_ir_dir();

#endif

#ifdef IR_REMOTE_IMPL
#ifndef IR_REMOTE_IMPL_GUARD
#define IR_REMOTE_IMPL_GUARD

#include "joystick.ino"
#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 13;

const uint8_t IR_CMD_UP = 0x18;     // 2
const uint8_t IR_CMD_LEFT = 0x08;   // 4
const uint8_t IR_CMD_SELECT = 0x1C; // 5
const uint8_t IR_CMD_RIGHT = 0x5A;  // 6
const uint8_t IR_CMD_DOWN = 0x52;   // 8

const uint32_t IR_DEBOUNCE_MS = 180;
uint32_t last_ir_ms = 0;

void init_ir_remote() {
    pinMode(IR_RECEIVE_PIN, INPUT);
    IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
    Serial.println("IR remote initialized");
}

joystick_dir_t get_ir_dir() {
    if (!IrReceiver.decode()) {
        return joystick_dir_t::None;
    }

    uint32_t now = millis();
    uint8_t command = IrReceiver.decodedIRData.command;

    IrReceiver.resume();

    if (now - last_ir_ms < IR_DEBOUNCE_MS) {
        return joystick_dir_t::None;
    }
    last_ir_ms = now;

    switch (command) {
    case IR_CMD_UP:
        return joystick_dir_t::Up;
    case IR_CMD_DOWN:
        return joystick_dir_t::Down;
    case IR_CMD_LEFT:
        return joystick_dir_t::Left;
    case IR_CMD_RIGHT:
    case IR_CMD_SELECT:
        return joystick_dir_t::Right;
    default:
        return joystick_dir_t::None;
    }
}

#endif
#endif