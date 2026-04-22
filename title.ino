#ifndef TITLE_INO
#define TITLE_INO
#include "scene.ino"

void to_title_scene();
void update_title_scene(uint32_t time_diff);

#endif
#ifdef TITLE_IMPL
#ifndef TITLE_IMPL_GUARD
#define TITLE_IMPL_GUARD

// score.ino and team.ino are included after title.ino, so forward
// declare the transitions we invoke. to_game_scene is already
// visible via game.ino above us in the include chain.
void to_score_scene();
void to_team_scene();

// scene state -- reset by to_title_scene so re-entry starts clean.
Animation title_animation = {.frame_ms = 333, .frame_count = 9, .repeat = 1};
int title_list_idx = 0;
joystick_dir_t title_prev_js_dir = joystick_dir_t::None;

void to_title_scene() {
    current_scene = scene_e::Title;
    clear_screen();
    reset_animation(&title_animation);
    title_list_idx = 0;
    title_prev_js_dir = get_joystick_dir();
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
    // title text animation
    update_animation(&title_animation, time_diff);

    write_string(0, 0, "SNAKE", 5);
    write_string(1, 0, "GAME", 4);
    title_lower(title_animation.frame);
    title_lower(title_animation.frame + 1);
    title_lower(title_animation.frame + 2);

    // title list

    joystick_dir_t js_dir = get_joystick_dir();
    if (js_dir != title_prev_js_dir) {
        title_prev_js_dir = js_dir;

        switch (js_dir) {
        case joystick_dir_t::Up:
            title_list_idx -= 1;
            break;
        case joystick_dir_t::Down:
            title_list_idx += 1;
            break;
        case joystick_dir_t::Right:
            switch (title_list_idx) {
            case 0:
                to_game_scene();
                break;
            case 1:
                to_score_scene();
                break;
            case 2:
                to_team_scene();
                break;
            }
            return;
        }

        title_list_idx = constrain(title_list_idx, 0, 2);
    }

    const char *LIST_ENTRY[4] = {"start", "score", "team ", "     "};

    draw_arrow(15, title_list_idx, 2);

    write_custom_char(0, 15 - 6, custom_char_e::SelectArrow);
    write_string(0, 15 - 5, (char *)LIST_ENTRY[title_list_idx], 5);
    write_string(1, 15 - 5, (char *)LIST_ENTRY[title_list_idx + 1], 5);
}

#endif
#endif
