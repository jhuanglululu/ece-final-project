#ifndef GAME_INO
#define GAME_INO
#include "scene.ino"

enum class game_state_e {
    Ongoing,
    Win,
    Lose,
};

game_state_e update_snake();
void reset_game();
void to_game_scene();
void update_game_scene(uint32_t time_diff);

#endif
#ifdef GAME_IMPL
#ifndef GAME_IMPL_GUARD
#define GAME_IMPL_GUARD

/* input pins */
const int START_PIN = 14; // button: start/reset game
const int SPEED_PIN = 39; // potentiometer: snake speed

/* output pins */
const int GAMESTATE_PIN = 26; // led: gamestate
const int BUZZER_PIN = 12;    // buzzer: sound effect

/* 7 segment display */
const int STCP_PIN = 4; // brown IO4
const int SHCP_PIN = 5; // blue IO5
const int DS_PIN = 2;   // purple io 2

/* board information */
const int rows = 4;
const int columns = 8;
const int max_snake_len = 32;

/* joystick */
joystick_dir_t last_move = joystick_dir_t::Right;
joystick_dir_t pending_move = joystick_dir_t::Right;

/* snake */
int snake[32] = {0};
int head = 1;
int tail = 0;

// snake length(score), start at 1
int score = 1;

bool is_opposite(joystick_dir_t a, joystick_dir_t b) {
    return (a == joystick_dir_t::Up && b == joystick_dir_t::Down) ||
           (a == joystick_dir_t::Down && b == joystick_dir_t::Up) ||
           (a == joystick_dir_t::Left && b == joystick_dir_t::Right) ||
           (a == joystick_dir_t::Right && b == joystick_dir_t::Left);
}

/* fruit spawning logic */

// fruit index
int fruit = 0;
// number of empty space
int n_available = 32;
// list of empty space(unordered)
char available[32] = {0};

uint32_t last_update = 0;
uint32_t sec_per_update = 400;

// index into variable
char avail_idx[32] = {0};

void reset_available() {
    n_available = max_snake_len;
    for (int i = 0; i < max_snake_len; i++) {
        available[i] = i;
        avail_idx[i] = i;
    }
}

void add_available(int cell) {
    available[n_available] = cell;
    avail_idx[cell] = n_available;
    n_available += 1;
}

void remove_available(int cell) {
    int i = avail_idx[cell];
    int last = available[n_available - 1];
    available[i] = last;
    avail_idx[last] = i;
    avail_idx[cell] = -1;
    n_available -= 1;
}

void spawn_fruit() {
    int idx = rand() % n_available;
    fruit = available[idx];
    remove_available(fruit);
}

/* snake logic */

void reset_snake() {
    for (int i = 0; i < max_snake_len; i++)
        snake[i] = -1;

    snake[0] = 0;
    head = 1;
    tail = 0;
}

game_state_e update_snake() {
    int next;
    int prev = snake[(head - 1 + max_snake_len) % max_snake_len];
    const int rows = max_snake_len / columns;

    // check if next block is out of bound
    switch (last_move) {
    case joystick_dir_t::Up:
        if (prev / columns == 0)
            return game_state_e::Lose;
        next = prev - columns;
        break;
    case joystick_dir_t::Down:
        if (prev / columns == rows - 1)
            return game_state_e::Lose;
        next = prev + columns;
        break;
    case joystick_dir_t::Left:
        if (prev % columns == 0)
            return game_state_e::Lose;
        next = prev - 1;
        break;
    case joystick_dir_t::Right:
        if (prev % columns == columns - 1)
            return game_state_e::Lose;
        next = prev + 1;
        break;
    }

    bool ate = (next == fruit);

    if (ate) {
        if (n_available == 0) {
            // no more empty space
            score += 1;
            return game_state_e::Win;
        }
        spawn_fruit();
        score += 1;
    } else {
        add_available(snake[tail]);
        tail = (tail + 1) % max_snake_len;
    }

    for (int i = tail; i != head; i = (i + 1) % max_snake_len) {
        // hitting body
        if (snake[i] == next)
            return game_state_e::Lose;
    }

    // fruit cells are removed from the pool on spawn; only mark
    // the head cell occupied when we moved into empty space.
    if (!ate)
        remove_available(next);

    snake[head] = next;
    head = (head + 1) % max_snake_len;
    return game_state_e::Ongoing;
}

void reset_game() {
    reset_snake();
    reset_available();

    for (int i = tail; i != head; i = (i + 1) % max_snake_len)
        remove_available(snake[i]);

    spawn_fruit();

    last_move = joystick_dir_t::Right;
    pending_move = joystick_dir_t::Right;

    score = 1;
    last_update = 0;
}

void to_game_scene() {
    current_scene = scene_e::Game;
    clear_screen();
    reset_game();
}

/* drawing */

enum class board_cell_e : uint8_t { Air, Snake, Fruit };

void draw_game() {
    write_custom_char(0, 7, custom_char_e::Border);
    write_custom_char(1, 7, custom_char_e::Border);

    write_string(0, 0, "score:", 6);

    board_cell_e board[32];
    for (int i = 0; i < 32; i++)
        board[i] = board_cell_e::Air;

    for (int i = tail; i != head; i = (i + 1) % max_snake_len)
        board[snake[i]] = board_cell_e::Snake;

    board[fruit] = board_cell_e::Fruit;

    for (int lcd_r = 0; lcd_r < 2; lcd_r++) {
        for (int lcd_c = 0; lcd_c < columns; lcd_c++) {
            board_cell_e top = board[(lcd_r * 2) * columns + lcd_c];
            board_cell_e bot = board[(lcd_r * 2 + 1) * columns + lcd_c];

            if (top == board_cell_e::Air && bot == board_cell_e::Air) {
                write_ascii_char(lcd_r, 8 + lcd_c, ' ');
                continue;
            }

            custom_char_e glyph;
            if (top == board_cell_e::Snake && bot == board_cell_e::Snake)
                glyph = custom_char_e::SnakeSnake;
            else if (top == board_cell_e::Snake && bot == board_cell_e::Air)
                glyph = custom_char_e::SnakeAir;
            else if (top == board_cell_e::Air && bot == board_cell_e::Snake)
                glyph = custom_char_e::AirSnake;
            else if (top == board_cell_e::Fruit && bot == board_cell_e::Air)
                glyph = custom_char_e::FruitAir;
            else if (top == board_cell_e::Air && bot == board_cell_e::Fruit)
                glyph = custom_char_e::AirFruit;
            else if (top == board_cell_e::Snake && bot == board_cell_e::Fruit)
                glyph = custom_char_e::SnakeFruit;
            else 
                glyph = custom_char_e::FruitSnake;

            write_custom_char(lcd_r, 8 + lcd_c, glyph);
        }
    }
}

void to_title_scene();
void add_score(int score);

void update_game_scene(uint32_t time_diff) {
    joystick_dir_t dir = get_joystick_dir();
    if (dir != joystick_dir_t::None)
        pending_move = dir;

    last_update += time_diff;
    while (last_update > sec_per_update) {
        if (!is_opposite(pending_move, last_move))
            last_move = pending_move;

        game_state_e result = update_snake();
        last_update -= sec_per_update;
        if (result != game_state_e::Ongoing) {
            add_score(score);
            to_title_scene();
            return;
        }
    }

    draw_game();
}

#endif
#endif
