#ifndef CUSTOM_CHARS_INO
#define CUSTOM_CHARS_INO
// custom characters
enum class custom_char_e : uint8_t {
    UpArrow,
    DownArrow,
    SelectArrow,
    Trophy,
    InverseC,
    TypingCursor,
    SnakeSnake,
    SnakeAir,
    AirSnake,
    FruitAir,
    AirFruit,
    SnakeFruit,
    FruitSnake,
    Border,
    Count,
};

const int custom_char_count = (int)custom_char_e::Count;
uint8_t *custom_char_bitmap(custom_char_e g);

#endif
#ifdef CUSTOM_CHARS_IMPL
#ifndef CUSTOM_CHARS_IMPL_GUARD
#define CUSTOM_CHARS_IMPL_GUARD

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

byte inverse_c[8] = {
    0b01110, 0b10001, 0b00001, 0b00001, 0b00001, 0b10001, 0b01110, 0b00000,
};

byte typing_cursor[8] = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111,
};

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

byte border[8] = {
    0b00001, 0b00001, 0b00001, 0b00001, 0b00001, 0b00001, 0b00001, 0b00001,
};

uint8_t *custom_char_bitmap(custom_char_e g) {
    switch (g) {
    case custom_char_e::UpArrow:
        return up_arrow;
    case custom_char_e::DownArrow:
        return down_arrow;
    case custom_char_e::SelectArrow:
        return select_arrow;
    case custom_char_e::Trophy:
        return trophy;
    case custom_char_e::InverseC:
        return inverse_c;
    case custom_char_e::TypingCursor:
        return typing_cursor;
    case custom_char_e::SnakeSnake:
        return snake_snake;
    case custom_char_e::SnakeAir:
        return snake_air;
    case custom_char_e::AirSnake:
        return air_snake;
    case custom_char_e::FruitAir:
        return fruit_air;
    case custom_char_e::AirFruit:
        return air_fruit;
    case custom_char_e::SnakeFruit:
        return snake_fruit;
    case custom_char_e::FruitSnake:
        return fruit_snake;
    case custom_char_e::Border:
        return border;
    default:
        return up_arrow;
    }
}
#endif
#endif
