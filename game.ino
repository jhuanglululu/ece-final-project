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

#include "joystick.ino"
#include "buzzer.ino"
#include "screen.ino"
#include "score.ino"
#include "title.ino"
#include "iot_score.ino"

/* input pins */
const int SPEED_PIN = 39; // potentiometer: snake speed

/* output pins */
const int BUZZER_PIN = 12;    // buzzer: sound effect

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

void spawn_fruit() {
    bool occupied[max_snake_len] = {false};
    for (int i = tail; i != head; i = (i + 1) % max_snake_len)
        occupied[snake[i]] = true;

    int available[max_snake_len];
    int n_available = 0;
    for (int i = 0; i < max_snake_len; i++)
        if (!occupied[i])
            available[n_available++] = i;

    fruit = available[rand() % n_available];
}

/* snake logic */

void reset_snake() {
    snake[0] = 0;
    head = 1;
    tail = 0;
}

bool is_body(int idx) {
    for (int i = tail; i != head; i = (i + 1) % max_snake_len)
        if (snake[i] == idx)
            return true;
    return false;
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

    if (!ate)
        tail = (tail + 1) % max_snake_len;

    if (is_body(next))
        return game_state_e::Lose;

    snake[head] = next;
    head = (head + 1) % max_snake_len;

    if (ate) {
        play_eating_sound();
        
        score += 1;
        if (score == max_snake_len)
            return game_state_e::Win;
        set_score_display(score);
        spawn_fruit();
    }

    return game_state_e::Ongoing;
}


/* drawing */

enum class board_cell_e : uint8_t { Air, Snake, Fruit };

void draw_game() {
    write_custom_char(0, 7, custom_char_e::Border);
    write_custom_char(1, 7, custom_char_e::Border);

    write_string(0, 0, "score:", 6);
    if (score >= 10) {
        write_ascii_char(1, 0, score / 10 + '0');
        write_ascii_char(1, 1, score % 10 + '0');
    } else {
        write_ascii_char(1, 0, score + '0');
    }

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

const int end_screen_time = 1500;
int end_screen_count_down = 0;
uint32_t last_update = 0;

void reset_game() {
    reset_snake();
    spawn_fruit();

    last_move = joystick_dir_t::Right;
    pending_move = joystick_dir_t::Right;

    score = 1;
    last_update = 0;
}

void to_game_scene() {
    current_scene = scene_e::Game;
    end_screen_count_down = 0;
    clear_screen();
    reset_game();
}

void update_game_scene(uint32_t time_diff) {
    if (end_screen_count_down > 0) {
        end_screen_count_down -= time_diff;

        if (end_screen_count_down <= 0)
            to_title_scene();

        return;
    }

    joystick_dir_t dir = get_joystick_dir();

    if (dir != joystick_dir_t::None)
        pending_move = dir;

    int pot_value = analogRead(SPEED_PIN); // replace with potentiometer value
    int time_between_update = map(pot_value, 0, 4095, 200, 1000);

    last_update += time_diff;
    while (last_update > time_between_update) {
        if (!is_opposite(pending_move, last_move))
            last_move = pending_move;

        game_state_e result = update_snake();
        last_update -= time_between_update;

        if (result != game_state_e::Ongoing) {
            if (result == game_state_e::Lose)
                write_string(0, 0, "lose  ", 6);
            else
                write_string(0, 0, "win   ", 6);

            add_score(score);
            publish_score(score);
            end_screen_count_down = end_screen_time;
            return;
        }
    }

    draw_game();
}

#endif
#endif
