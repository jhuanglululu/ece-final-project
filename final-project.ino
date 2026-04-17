#include <LiquidCrystal_I2C.h>

/*
 * project notes:
 *  - use uint32_t for time related values
 */

/* input pins */

const int START_PIN = 14;      // button: start/reset game
const int JOYSTICK_X_PIN = 35; // joystick: controll snake
const int JOYSTICK_Y_PIN = 34; //
const int SPEED_PIN = 39;      // potentiometer: snake speed

/* output pins */
const int GAMESTATE_PIN = 26; // led: gamestate
const int BUZZER_PIN = 12;    // buzzer: sound effect

/* 7 segment display */
const int STCP_PIN = 4; // brown IO4
const int SHCP_PIN = 5; // blue IO5
const int DS_PIN = 2;   // purple io 2

/* screen, IO21 & IO 22 */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* animation helper */

struct Animation {
    uint32_t timer;
    uint32_t frame_ms;
    int frame;
    int frame_count;
    int repeat;
};

void update_animation(Animation *anim, uint32_t time_diff) {
    if (anim->repeat == 0 && anim->frame == anim->frame_count - 1)
        return;
    else if (anim->repeat == 0 && anim->frame >= anim->frame_count)
        anim->frame = anim->frame_count - 1;

    anim->timer += time_diff;
    anim->frame += anim->timer / anim->frame_ms;
    anim->timer %= anim->frame_ms;
    anim->frame %= anim->frame_count;
}

void reset_animation(Animation *anim) {
    anim->timer = 0;
    anim->frame = 0;
}

/* custom characters */

enum class CustomCharProfile {
    Title,
    Game,
    Chuang,
};

byte up_arrow[8] = {
    0b00000, 0b00000, 0b00000, 0b00100, 0b01110, 0b01010, 0b00000, 0b00000,
};

byte down_arrow[8] = {
    0b00000, 0b00000, 0b01010, 0b01110, 0b00100, 0b00000, 0b00000, 0b00000,
};

byte select_arrow[8] = {
    0b00000, 0b00000, 0b01000, 0b01100, 0b11110, 0b01100, 0b01000, 0b00000,
};

byte trophy[8] = {
    0b00000, 0b01010, 0b01110, 0b01110, 0b00100, 0b00100, 0b01110, 0b00000,
};

/* chuang's team member page */

byte inverse_c[8] = {
    0b01110, 0b10001, 0b00001, 0b00001, 0b00001, 0b10001, 0b01110, 0b00000,
};

byte typing_cursor[8] = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111,
};

/* game custom chars */

byte snake_snake[8] = {
    0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111,
};

byte snake_air[8] = {
    0b11111, 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000,
};

byte air_snake[8] = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111,
};

byte fruit_air[8] = {
    0b00100, 0b01110, 0b01110, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000,
};

byte air_fruit[8] = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b01110, 0b01110, 0b00000,
};

byte snake_fruit[8] = {
    0b11111, 0b11111, 0b11111, 0b11111, 0b00100, 0b01110, 0b01110, 0b00000,
};

byte fruit_snake[8] = {
    0b00100, 0b01110, 0b01110, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111,
};

byte heart[8] = {
    0b00000, 0b00000, 0b01010, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000,
};

enum class CustomChar : uint8_t {
    // title
    UpArrow = 0,
    DownArrow = 1,
    SelectArrow = 2,
    // chuang
    InverseC = 2,
    TypingCursor = 3,
    // game
    SnakeSnake = 0,
    SnakeAir = 1,
    AirSnake = 2,
    FruitAir = 3,
    AirFruit = 4,
    SnakeFruit = 5,
    FruitSnake = 6,
    Heart = 7,
};

void set_custom_chars(CustomCharProfile profile);

void set_custom_chars(CustomCharProfile profile) {
    switch (profile) {
    case CustomCharProfile::Title:
        lcd.createChar((uint8_t)CustomChar::UpArrow, up_arrow);
        lcd.createChar((uint8_t)CustomChar::DownArrow, down_arrow);
        lcd.createChar((uint8_t)CustomChar::SelectArrow, select_arrow);
        break;
    case CustomCharProfile::Chuang:
        lcd.createChar((uint8_t)CustomChar::UpArrow, up_arrow);
        lcd.createChar((uint8_t)CustomChar::DownArrow, down_arrow);
        lcd.createChar((uint8_t)CustomChar::SelectArrow, select_arrow);
        lcd.createChar((uint8_t)CustomChar::InverseC, inverse_c);
        lcd.createChar((uint8_t)CustomChar::TypingCursor, typing_cursor);
        break;
    case CustomCharProfile::Game:
        lcd.createChar((uint8_t)CustomChar::SnakeSnake, snake_snake);
        lcd.createChar((uint8_t)CustomChar::SnakeAir, snake_air);
        lcd.createChar((uint8_t)CustomChar::AirSnake, air_snake);
        lcd.createChar((uint8_t)CustomChar::FruitAir, fruit_air);
        lcd.createChar((uint8_t)CustomChar::AirFruit, air_fruit);
        lcd.createChar((uint8_t)CustomChar::SnakeFruit, snake_fruit);
        lcd.createChar((uint8_t)CustomChar::FruitSnake, fruit_snake);
        lcd.createChar((uint8_t)CustomChar::Heart, heart);
        break;
    }
}

/*
 * virtual screen
 *
 * store a buffer in the cpp program and only update characters
 * that change so we don't see flashing characters or lag
 *
 * because of the buffer, writing the same character to the same
 * cell every frame or doing multiple write to same cell in one
 * frame cost nothing
 */

enum class CellKind { Custom, Ascii };

struct Cell {
    CellKind kind;
    uint8_t value;
};

const int SCREEN_COLUMN = 16;
const int SCREEN_ROW = 2;

Cell cur_screen[SCREEN_ROW][SCREEN_COLUMN];
Cell next_screen[SCREEN_ROW][SCREEN_COLUMN];

int cell_equal(int r, int c) {
    Cell c1 = cur_screen[r][c];
    Cell c2 = next_screen[r][c];
    if (c1.kind == c2.kind && c1.value == c2.value)
        return 1;
    return 0;
}

void clear_screen() {
    for (int r = 0; r < SCREEN_ROW; r++) {
        for (int c = 0; c < SCREEN_COLUMN; c++) {
            next_screen[r][c] = {CellKind::Ascii, ' '};
        }
    }
}

void draw_screen() {
    for (int r = 0; r < SCREEN_ROW; r++) {
        for (int c = 0; c < SCREEN_COLUMN; c++) {
            if (cell_equal(r, c))
                continue;

            Cell cell = next_screen[r][c];
            cur_screen[r][c] = cell;

            lcd.setCursor(c, r);
            if (cell.kind == CellKind::Ascii)
                lcd.write((char)cell.value);
            else
                lcd.write((uint8_t)cell.value);
        }
    }
}

void write_custom_char(int r, int c, CustomChar ch);
void write_custom_char(int r, int c, CustomChar ch) { next_screen[r][c] = {CellKind::Custom, (uint8_t)ch}; }

void write_ascii_char(int r, int c, char ch) { next_screen[r][c] = {CellKind::Ascii, (uint8_t)ch}; }

void write_string(int r, int c, char *str, int len) {
    for (int i = 0; i < len; i++) {
        if (c + i >= SCREEN_COLUMN)
            break;
        next_screen[r][c + i] = {CellKind::Ascii, (uint8_t)str[i]};
    }
}

/* joystick section */

enum class JoystickDir { None, Up, Down, Left, Right };

const int JOYSTICK_OFFSET = 4096 / 2;
const int JOYSTICK_THRESH = 512;

// arduino generates a header for this function that
JoystickDir get_joystick_dir();

JoystickDir get_joystick_dir() {
    int x = analogRead(JOYSTICK_X_PIN) - JOYSTICK_OFFSET;
    int y = analogRead(JOYSTICK_Y_PIN) - JOYSTICK_OFFSET;

    if (abs(x) < JOYSTICK_THRESH && abs(y) < JOYSTICK_THRESH)
        return JoystickDir::None;
    if (abs(x) < abs(y) && y < 0)
        return JoystickDir::Up;
    else if (abs(x) < abs(y))
        return JoystickDir::Down;
    else if (x < 0)
        return JoystickDir::Left;
    else
        return JoystickDir::Right;
}

/* scenes */

enum class Scene { Title, Game, Team };

Scene current_scene = Scene::Title;

/* title scene section */

void to_title_scene() {
    current_scene = Scene::Title;
    clear_screen();
    set_custom_chars(CustomCharProfile::Title);
}

void title_lower(int c_idx) {
    const char *LOWER_TITLE = "snakegame";
    c_idx %= 9;

    if (c_idx < 5)
        write_ascii_char(0, c_idx, LOWER_TITLE[c_idx]);
    else
        write_ascii_char(1, c_idx - 5, LOWER_TITLE[c_idx]);
}

void update_title_scene(uint32_t time_diff) {
    // const uint32_t FRAME_MS = 333;
    // static uint32_t animation_timer = 0;
    // static int animation_frame = 0;

    // animation_timer += time_diff;
    // animation_frame += animation_timer / FRAME_MS;
    // animation_timer %= FRAME_MS;
    // animation_frame %= 9;

    // title text section
    static Animation animation = {.frame_ms = 333, .frame_count = 9, .repeat = 1};
    update_animation(&animation, time_diff);

    write_string(0, 0, "SNAKE", 5);
    write_string(1, 0, "GAME", 4);
    title_lower(animation.frame);
    title_lower(animation.frame + 1);
    title_lower(animation.frame + 2);

    // title list

    static int list_idx = 0;

    static JoystickDir prev_js_dir = JoystickDir::None;
    JoystickDir js_dir = get_joystick_dir();
    if (js_dir != prev_js_dir) {
        prev_js_dir = js_dir;
        switch (js_dir) {
        case JoystickDir::Up:
            list_idx -= 1;
            break;
        case JoystickDir::Down:
            list_idx += 1;
            break;
        case JoystickDir::Right:
            if (list_idx == 2)
                to_team_scene();
            list_idx = 0;
            return;
        }

        list_idx = constrain(list_idx, 0, 2);
    }

    const char *LIST_ENTRY[4] = {"start", "score", "team ", "     "};

    write_custom_char(0, 15, CustomChar::UpArrow);
    write_custom_char(1, 15, CustomChar::DownArrow);
    if (list_idx == 0)
        write_ascii_char(0, 15, ' ');
    if (list_idx >= 2)
        write_ascii_char(1, 15, ' ');

    write_custom_char(0, 15 - 6, CustomChar::SelectArrow);
    write_string(0, 15 - 5, (char *)LIST_ENTRY[list_idx], 5);
    write_string(1, 15 - 5, (char *)LIST_ENTRY[list_idx + 1], 5);
}

void to_team_scene() {
    current_scene = Scene::Team;
    to_chuang_scene();
}

void update_team_scene(uint32_t time_diff) {
    static int member_idx = 0;

    static JoystickDir prev_js_dir = JoystickDir::None;
    JoystickDir js_dir = get_joystick_dir();

    if (js_dir != prev_js_dir) {
        prev_js_dir = js_dir;
        switch (js_dir) {
        case JoystickDir::Up:
            member_idx -= 1;
            break;
        case JoystickDir::Down:
            member_idx += 1;
            break;
        case JoystickDir::Left:
            member_idx = 0;
            to_title_scene();
            return;
        }

        switch (member_idx) {
        case 0:
            to_chuang_scene();
            break;
        }
        member_idx = constrain(member_idx, 0, 2);
    }

    write_custom_char(0, 15, CustomChar::UpArrow);
    write_custom_char(1, 15, CustomChar::DownArrow);
    if (member_idx == 0)
        write_ascii_char(0, 15, ' ');
    if (member_idx >= 2)
        write_ascii_char(1, 15, ' ');

    switch (member_idx) {
    case 0:
        update_chuang_scene(time_diff);
        break;
    }
}

static Animation chuang_anim = {.frame_ms = 100, .frame_count = 19};

void to_chuang_scene() {
    clear_screen();
    set_custom_chars(CustomCharProfile::Chuang);
    reset_animation(&chuang_anim);
}

void update_chuang_scene(uint32_t time_diff) {
    update_animation(&chuang_anim, time_diff);

    write_string(0, 0, "       ", 7);
    if (chuang_anim.frame < 7) {
        write_string(0, 0, "Chuang", chuang_anim.frame);
        write_custom_char(0, 0, CustomChar::InverseC);
        write_custom_char(0, chuang_anim.frame, CustomChar::TypingCursor);
    } else if (chuang_anim.frame < 13) {
        write_string(0, 0, "Chuang", 12 - chuang_anim.frame);
        write_custom_char(0, 0, CustomChar::InverseC);
        write_custom_char(0, 12 - chuang_anim.frame, CustomChar::TypingCursor);
    } else if (chuang_anim.frame < 19) {
        write_string(0, 0, "Chuang", chuang_anim.frame - 12);
        if (chuang_anim.frame < 18)
            write_custom_char(0, chuang_anim.frame - 12, CustomChar::TypingCursor);
    }
}

/* setup and loop */

void setup() {
    /* initialize the lcd */
    lcd.init();
    lcd.backlight();
    lcd.clear();

    /* initialize inputs */
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);

    /* initialize the 7 segment display */
    pinMode(STCP_PIN, OUTPUT);
    pinMode(SHCP_PIN, OUTPUT);
    pinMode(DS_PIN, OUTPUT);

    set_custom_chars(CustomCharProfile::Title);
    clear_screen();
    to_title_scene();
    draw_screen();
}

void loop() {
    static uint32_t last_frame_ms = 0;
    uint32_t time_diff = millis() - last_frame_ms;
    last_frame_ms = millis();

    switch (current_scene) {
    case Scene::Title:
        update_title_scene(time_diff);
        break;
    case Scene::Team:
        update_team_scene(time_diff);
        break;
    }

    draw_screen();
    delay(33);
}
