#ifndef SCORE_INO
#define SCORE_INO

/* this file includes high score page logic */

#include "scene.ino"

void to_score_scene();
void add_score(int score);
void update_score_scene(uint32_t time_diff);

#endif
#ifdef SCORE_IMPL
#ifndef SCORE_IMPL_GUARD
#define SCORE_IMPL_GUARD

#include "joystick.ino"
#include "screen.ino"
#include "title.ino"

int score_idx = 0;
joystick_dir_t score_prev_js_dir = joystick_dir_t::None;

void to_score_scene() {
    current_scene = scene_e::Score;
    clear_screen();
    score_idx = 0;
}

const int MAX_SCORE_COUNT = 10;

int high_scores[MAX_SCORE_COUNT] = {0};
int score_len = 0;

void add_score(int score) {
    if (score < high_scores[9])
        return;

    int i = 0;
    while (i < MAX_SCORE_COUNT && high_scores[i] > score)
        i += 1;

    for (int rev_i = MAX_SCORE_COUNT - 1; rev_i > i; rev_i--)
        high_scores[rev_i] = high_scores[rev_i - 1];

    high_scores[i] = score;

    score_len = min(score_len + 1, MAX_SCORE_COUNT);
}

void draw_score(int row, int rank, int score) {
    if (rank == 1)
        write_custom_char(row, 1, custom_char_e::Trophy);

    if (rank < 10) {
        write_ascii_char(row, 0, ' ');
        write_ascii_char(row, 1, '0' + rank);
    } else {
        write_ascii_char(row, 0, '0' + rank / 10);
        write_ascii_char(row, 1, '0' + rank % 10);
    }

    write_ascii_char(row, 2, ':');
    write_ascii_char(row, 3, ' ');

    if (score < 10) {
        write_ascii_char(row, 4, ' ');
        write_ascii_char(row, 5, '0' + score);
    } else {
        write_ascii_char(row, 4, '0' + score / 10);
        write_ascii_char(row, 5, '0' + score % 10);
    }
}

void update_score_scene(uint32_t time_diff) {
    joystick_dir_t js_dir = get_joystick_dir();
    if (js_dir != score_prev_js_dir) {
        score_prev_js_dir = js_dir;
        switch (js_dir) {
        case joystick_dir_t::Left:
            to_title_scene();
            return;
        case joystick_dir_t::Up:
            score_idx -= 1;
            break;
        case joystick_dir_t::Down:
            score_idx += 1;
            break;
        }
    }

    if (score_len <= 0) {
        write_string(0, 0, "No scores", 9);
        return;
    }

    score_idx = constrain(score_idx, 0, max(score_len - 2, 0));

    draw_arrow(15, score_idx, score_len - 1);

    draw_score(0, score_idx + 1, high_scores[score_idx]);
    if (score_idx + 1 < score_len)
        draw_score(1, score_idx + 2, high_scores[score_idx + 1]);
    else
        write_string(1, 0, "      ", 6);
}

#endif
#endif
