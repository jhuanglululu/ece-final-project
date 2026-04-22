#define ANIMATION_IMPL
#include "animation.ino"
#define CUSTOM_CHARS_IMPL
#include "custom_chars.ino"
#define JOYSTICK_IMPL
#include "joystick.ino"
#define SCREEN_IMPL
#include "screen.ino"
#define SCENE_IMPL
#include "scene.ino"
#define GAME_IMPL
#include "game.ino"
#define TITLE_IMPL
#include "title.ino"
#define SCORE_IMPL
#include "score.ino"
#define TEAM_IMPL
#include "team.ino"

/*
 * project notes:
 *  - use uint32_t for time related values
 */

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

    init_lcd_mem_slot();
    clear_screen();
    draw_screen();
    to_title_scene();
}

void loop() {
    static uint32_t last_frame_ms = 0;
    uint32_t time_diff = millis() - last_frame_ms;
    last_frame_ms = millis();

    switch (current_scene) {
    case scene_e::Title:
        update_title_scene(time_diff);
        break;
    case scene_e::Score:
        update_score_scene(time_diff);
        break;
    case scene_e::Team:
        update_team_scene(time_diff);
        break;
    case scene_e::Game:
        update_game_scene(time_diff);
        break;
    }

    draw_screen();
    delay(max(0, (int)(33 + last_frame_ms - millis())));
}
