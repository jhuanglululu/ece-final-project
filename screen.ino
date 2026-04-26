#ifndef SCREEN_INO
#define SCREEN_INO

/* this file includes screen rendering logic */

#include "custom_chars.ino"
#include <LiquidCrystal_I2C.h>

void init_lcd_mem_slot();
void clear_screen();
void draw_screen();
void write_custom_char(int r, int c, custom_char_e ch);
void write_ascii_char(int r, int c, char ch);
void write_string(int r, int c, char *str, int len);
void draw_arrow(int col, int idx, int len);

#endif
#ifdef SCREEN_IMPL
#ifndef SCREEN_IMPL_GUARD
#define SCREEN_IMPL_GUARD

#include <LiquidCrystal_I2C.h>

// screen, IO21 & IO 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

struct lcd_mem_slot_t {
    int last_used;
    custom_char_e c;
};

// array that holds what is in the lcd custom character memory
lcd_mem_slot_t lcd_mems[8];

void init_lcd_mem_slot() {
    for (int i = 0; i < 8; i++) {
        lcd_mems[i] = {.last_used = -1, .c = custom_char_e::UpArrow};
    }
}

/*
 * virtual screen
 *
 * store a buffer in esp32 and only update characters that change
 * so we don't see flashing characters or lag
 *
 * because of the buffer, writing the same character to the same
 * cell every frame or doing multiple write to same cell in one
 * frame cost nothing
 * 
 * in other part of the project, when something is written with
 * - write_ascii_char(...),
 * - write_string(...)
 * - write_custom_char(...)
 * the characters are written to the virtual screen, not the lcd
 */

enum class cell_kind_e { Custom, Ascii };

struct cell_t {
    cell_kind_e kind;
    uint8_t value;
};

const int SCREEN_COLUMN = 16;
const int SCREEN_ROW = 2;

cell_t cur_screen[2][16];
cell_t next_screen[2][16];

int cell_equal(int r, int c) {
    cell_t c1 = cur_screen[r][c];
    cell_t c2 = next_screen[r][c];
    if (c1.kind == c2.kind && c1.value == c2.value)
        return 1;
    return 0;
}

void clear_screen() {
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 16; c++) {
            next_screen[r][c] = {cell_kind_e::Ascii, ' '};
        }
    }
}

// although this function looks long, it is actually really 
// cheap to run since the screen is small
void draw_screen() {
    static uint32_t counter = 0;

    counter += 1;

    // 1st pass: find which custom char does next_screen need
    bool needed[custom_char_count] = {false};
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 16; c++) {
            cell_t cell = next_screen[r][c];
            if (cell.kind == cell_kind_e::Custom)
                needed[cell.value] = true;
        }
    }

    // 2nd pass: keep slots whose char is still needed, build char->slot map
    int char_to_slot[custom_char_count];
    for (int i = 0; i < custom_char_count; i++)
        char_to_slot[i] = -1;

    bool slot_kept[8] = {false};
    for (int s = 0; s < 8; s++) {
        lcd_mem_slot_t slot = lcd_mems[s];
        int c = (int)slot.c;
        // if the slot is not empty and we need it
        if (slot.last_used != -1 && needed[c]) {
            slot_kept[s] = true;
            char_to_slot[c] = s;
            lcd_mems[s].last_used = counter;
        }
    }

    // 3rd pass: assign needed chars without a slot
    for (int c_idx = 0; c_idx < custom_char_count; c_idx++) {
        // if the character is not need or has a slot
        if (!needed[c_idx] || char_to_slot[c_idx] != -1)
            continue;

        // best slot is either empty or has not been used for a while
        int best_slot = -1;
        for (int s = 0; s < 8; s++) {
            // someone else is using, skip
            if (slot_kept[s])
                continue;

            // slot is empty, take this one
            if (lcd_mems[s].last_used == -1) {
                best_slot = s;
                break;
            }

            // no best slot yet
            if (best_slot == -1)
                best_slot = s;

            // or this slot is not used for longer
            else if (lcd_mems[s].last_used < lcd_mems[best_slot].last_used)
                best_slot = s;
        }

        // write the slot to lcd
        lcd.createChar(best_slot, custom_char_bitmap((custom_char_e)c_idx));

        lcd_mems[best_slot].c = (custom_char_e)c_idx;
        lcd_mems[best_slot].last_used = counter;
        slot_kept[best_slot] = true;
        char_to_slot[c_idx] = best_slot;
    }

    // 4th pass 4: write changed cells
    for (int r = 0; r < SCREEN_ROW; r++) {
        for (int c = 0; c < SCREEN_COLUMN; c++) {
            if (cell_equal(r, c))
                continue;

            cell_t cell = next_screen[r][c];
            cur_screen[r][c] = cell;

            lcd.setCursor(c, r);
            if (cell.kind == cell_kind_e::Ascii)
                lcd.write((char)cell.value);
            else
                lcd.write((uint8_t)char_to_slot[cell.value]);
        }
    }
}

void write_custom_char(int r, int c, custom_char_e ch) { next_screen[r][c] = {cell_kind_e::Custom, (uint8_t)ch}; }

void write_ascii_char(int r, int c, char ch) { next_screen[r][c] = {cell_kind_e::Ascii, (uint8_t)ch}; }

// call write_ascii_char multiple time to write the string
void write_string(int r, int c, char *str, int len) {
    for (int i = 0; i < len; i++) {
        if (c + i >= SCREEN_COLUMN)
            break;
        next_screen[r][c + i] = {cell_kind_e::Ascii, (uint8_t)str[i]};
    }
}

// helper function for drawing arrow since is it use multiple times
void draw_arrow(int col, int idx, int len) {
    if (idx > 0 && len > 1)
        write_custom_char(0, col, custom_char_e::UpArrow);
    else
        write_ascii_char(0, col, ' ');

    if (idx < len - 1)
        write_custom_char(1, col, custom_char_e::DownArrow);
    else
        write_ascii_char(1, col, ' ');
}
#endif
#endif
