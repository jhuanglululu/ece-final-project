#ifndef TEAM_INO
#define TEAM_INO
#include "scene.ino"

void to_team_scene();
void update_team_scene(uint32_t time_diff);

#endif
#ifdef TEAM_IMPL
#ifndef TEAM_IMPL_GUARD
#define TEAM_IMPL_GUARD

#include "joystick.ino"
#include "screen.ino"
#include "title.ino"

void to_team_scene() {
    current_scene = scene_e::Team;
    clear_screen();
}

void update_team_scene(uint32_t time_diff) {
    if (get_joystick_dir() == joystick_dir_t::Left) {
        to_title_scene();
        return;
    }

    write_string(0, 0, "Made By", 7);
}

#endif
#endif
