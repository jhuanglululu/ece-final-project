#ifndef TEAM_INO
#define TEAM_INO
#include "scene.ino"

void to_team_scene();
void update_team_scene(uint32_t time_diff);

#endif
#ifdef TEAM_IMPL
#ifndef TEAM_IMPL_GUARD
#define TEAM_IMPL_GUARD

#include "buzzer.ino"
#include "joystick.ino"
#include "screen.ino"
#include "title.ino"

int team_list_idx = 0;
joystick_dir_t team_prev_js_dir = joystick_dir_t::None;

Animation train_anim = {.frame_ms = 200, .frame_count = 85, .repeat = 1};

void to_team_scene() {
    current_scene = scene_e::Team;
    team_list_idx = 0;
    clear_screen();
}

void update_team_scene(uint32_t time_diff) {
    clear_screen();

    joystick_dir_t js_dir = get_joystick_dir();
    if (js_dir != team_prev_js_dir) {
        team_prev_js_dir = js_dir;

        switch (js_dir) {
        case joystick_dir_t::Up:
            team_list_idx -= 1;
            break;
        case joystick_dir_t::Down:
            team_list_idx += 1;
            break;
        case joystick_dir_t::Left:
            to_title_scene();
            return;
        }

        team_list_idx = constrain(team_list_idx, 0, 2);
    }

    draw_arrow(15, team_list_idx, 3);

    switch (team_list_idx) {
    case 0:
        write_string(0, 0, "Abdi", 4);
        break;
    case 1:
        write_string(0, 0, "Antonio", 7);
        break;
    case 2:
        write_string(0, 0, "Chuang", 6);
        break;
    }
    update_animation(&train_anim, time_diff);

    for (int i = 0; i < 15; i++)
        write_custom_char(1, i, custom_char_e::TypingCursor);

    int train_frame = train_anim.frame % 5;
    int left_idx = train_anim.frame / 5 - 2;
    int mid_idx = left_idx + 1;
    int right_idx = mid_idx + 1;

    if (0 <= left_idx && left_idx < 15 && train_frame < 4)
        write_custom_char(1, left_idx, custom_char_e::TrainLeft0 + train_frame);
    if (0 <= mid_idx && mid_idx < 15)
        write_custom_char(1, mid_idx, custom_char_e::TrainMid0 + train_frame);
    if (0 <= right_idx && right_idx < 15 && 2 <= train_frame)
        write_custom_char(1, right_idx, custom_char_e::TrainRight2 + (train_frame - 2));

    // train whistle
    if (train_anim.frame == 40 || train_anim.frame == 40)
        play_eating_sound();
}

#endif
#endif
