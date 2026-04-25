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
#define IOT_SCORE_IMPL
#include "iot_score.ino"
#define GAME_IMPL
#include "game.ino"
#define TITLE_IMPL
#include "title.ino"
#define SCORE_IMPL
#include "score.ino"
#define TEAM_IMPL
#include "team.ino"

bool system_on = false;
bool last_button_state = LOW;

const int POWER_BUTTON_PIN = 14;
const int LED_PIN = 26;

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

    pinMode(POWER_BUTTON_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(LED_PIN, LOW);

    lcd.noDisplay();
    lcd.noBacklight();

    init_iot_score();

    init_lcd_mem_slot();
    clear_screen();
    draw_screen();
    to_title_scene();
}

void update_power_button() {
    bool button_state = digitalRead(POWER_BUTTON_PIN);

    if (button_state == LOW && last_button_state == HIGH) {
        system_on = !system_on;

        if (system_on) {
            lcd.display();
            lcd.backlight();
            digitalWrite(LED_PIN, HIGH);
            clear_screen();
            to_title_scene();
        } else {
            lcd.noDisplay();
            lcd.noBacklight();
            digitalWrite(LED_PIN, LOW);
        }

        delay(200);
    }

    last_button_state = button_state;
}

void loop() {
    static uint32_t last_frame_ms = 0;
    uint32_t time_diff = millis() - last_frame_ms;
    last_frame_ms = millis();

    update_power_button();

    if (!system_on) {
        delay(33);
        return;
    }

    update_iot_score(time_diff);

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
