#ifndef SCORE_INO
#define SCORE_INO
#include "scene.ino"

void to_score_scene();
void add_score(int score);
void update_score_scene(uint32_t time_diff);

#endif
#ifdef SCORE_IMPL
#ifndef SCORE_IMPL_GUARD
#define SCORE_IMPL_GUARD



// scroll position -- reset by to_score_scene on entry. high_scores
// and score_len are persistent records, not scene state, so they
// stay intact across re-entries.
int score_idx = 0;
joystick_dir_t score_prev_js_dir = joystick_dir_t::None;

void to_score_scene() {
    current_scene = scene_e::Score;
    clear_screen();
    score_idx = 0;
    // seed prev with the current stick so a held direction at
    // entry (e.g. Right held from the title menu) isn't treated
    // as a fresh press.
    score_prev_js_dir = get_joystick_dir();
}

int high_scores[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int score_len = 0;

// insert `score` into the sorted top-10 list, dropping the lowest.
void add_score(int score) {
    if (score < high_scores[9])
        return;

    int i = 0;
    while (i < 10 && high_scores[i] > score)
        i += 1;

    for (int rev_i = 9; rev_i > i; rev_i--)
        high_scores[rev_i] = high_scores[rev_i - 1];

    high_scores[i] = score;

    score_len = min(score_len + 1, 10);
}

// draw one row as "RR: SS" where RR is rank and SS is the score.
// trophy replaces the rank digit for the #1 spot.
void draw_score_row(int row, int rank, int score) {
    // rank (two chars, right-aligned)
    if (rank < 10) {
        write_ascii_char(row, 0, ' ');
        write_ascii_char(row, 1, '0' + rank);
    } else {
        write_ascii_char(row, 0, '0' + rank / 10);
        write_ascii_char(row, 1, '0' + rank % 10);
    }
    if (rank == 1)
        write_custom_char(row, 1, custom_char_e::Trophy);

    write_ascii_char(row, 2, ':');
    write_ascii_char(row, 3, ' ');

    // score (two chars, right-aligned, max value is max snake len 32)
    if (score < 10) {
        write_ascii_char(row, 4, ' ');
        write_ascii_char(row, 5, '0' + score);
    } else {
        write_ascii_char(row, 4, '0' + score / 10);
        write_ascii_char(row, 5, '0' + score % 10);
    }
}

void update_score_scene(uint32_t time_diff) {
    // edge-detect joystick so holding a direction scrolls exactly
    // one step; user must release before the next press registers.
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
        write_string(0, 0, (char *)"No scores", 9);
        return;
    }

    score_idx = constrain(score_idx, 0, max(score_len - 2, 0));

    // we render two rows at a time; draw_arrow hides its arrows
    // when there's nothing to scroll in that direction.
    draw_arrow(15, score_idx, score_len);

    draw_score_row(0, score_idx + 1, high_scores[score_idx]);
    if (score_idx + 1 < score_len)
        draw_score_row(1, score_idx + 2, high_scores[score_idx + 1]);
    else
        write_string(1, 0, (char *)"      ", 6);
}

#endif
#endif
