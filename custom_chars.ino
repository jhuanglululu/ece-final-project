#ifndef CUSTOM_CHARS_INO
#define CUSTOM_CHARS_INO
// custom characters
enum class custom_char_e : int {
    UpArrow,
    DownArrow,
    SelectArrow,
    Trophy,
    TypingCursor,
    SnakeSnake,
    SnakeAir,
    AirSnake,
    FruitAir,
    AirFruit,
    SnakeFruit,
    FruitSnake,
    Border,

    TrainLeft0,
    TrainLeft1,
    TrainLeft2,
    TrainLeft3,

    TrainMid0,
    TrainMid1,
    TrainMid2,
    TrainMid3,
    TrainMid4,

    TrainRight2,
    TrainRight3,
    TrainRight4,
    Count,
};

constexpr custom_char_e operator+(custom_char_e e, int n) {
    return static_cast<custom_char_e>(static_cast<int>(e) + n);
}

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

byte train_left_0[8] = {
    0b00000, 0b00000, 0b01111, 0b01101, 0b01111, 0b01111, 0b00100, 0b11111,
};

byte train_left_1[8] = {
    0b00000, 0b00000, 0b00111, 0b00110, 0b00111, 0b00111, 0b00010, 0b11111,
};

byte train_left_2[8] = {
    0b00000, 0b00000, 0b00011, 0b00011, 0b00011, 0b00011, 0b00001, 0b11111,
};

byte train_left_3[8] = {
    0b00000, 0b00000, 0b00001, 0b00001, 0b00001, 0b00001, 0b00000, 0b11111,
};

// byte train_left_4[8] is typing_cursor

byte train_mid_0[8] = {
    0b10000, 0b01000, 0b11110, 0b01000, 0b11110, 0b11110, 0b00100, 0b11111,
};

byte train_mid_1[8] = {
    0b01000, 0b00100, 0b11111, 0b10100, 0b11111, 0b11111, 0b00010, 0b11111,
};

byte train_mid_2[8] = {
    0b00100, 0b00010, 0b11111, 0b01010, 0b11111, 0b11111, 0b00001, 0b11111,
};

byte train_mid_3[8] = {
    0b00010, 0b00001, 0b11111, 0b10101, 0b11111, 0b11111, 0b10000, 0b11111,
};

byte train_mid_4[8] = {
    0b00001, 0b00000, 0b11111, 0b11010, 0b11111, 0b11111, 0b01000, 0b11111,
};

// byte train_right_0[8] is typing_cursor

// byte train_right_1[8] is typing_cursor

byte train_right_2[8] = {
    0b00000, 0b00000, 0b10000, 0b00000, 0b10000, 0b10000, 0b00000, 0b11111,
};

byte train_right_3[8] = {
    0b00000, 0b00000, 0b11000, 0b00000, 0b11000, 0b11000, 0b10000, 0b11111,
};

byte train_right_4[8] = {
    0b00000, 0b10000, 0b11100, 0b10000, 0b11100, 0b11100, 0b01000, 0b11111,
};

byte *custom_char_bitmap(custom_char_e g) {
    switch (g) {
    case custom_char_e::UpArrow:
        return up_arrow;
    case custom_char_e::DownArrow:
        return down_arrow;
    case custom_char_e::SelectArrow:
        return select_arrow;
    case custom_char_e::Trophy:
        return trophy;
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
    case custom_char_e::TrainLeft0:
        return train_left_0;
    case custom_char_e::TrainLeft1:
        return train_left_1;
    case custom_char_e::TrainLeft2:
        return train_left_2;
    case custom_char_e::TrainLeft3:
        return train_left_3;
    case custom_char_e::TrainMid0:
        return train_mid_0;
    case custom_char_e::TrainMid1:
        return train_mid_1;
    case custom_char_e::TrainMid2:
        return train_mid_2;
    case custom_char_e::TrainMid3:
        return train_mid_3;
    case custom_char_e::TrainMid4:
        return train_mid_4;
    case custom_char_e::TrainRight2:
        return train_right_2;
    case custom_char_e::TrainRight3:
        return train_right_3;
    case custom_char_e::TrainRight4:
        return train_right_4;
    default:
        return up_arrow;
    }
}
#endif
#endif
