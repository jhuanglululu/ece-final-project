/* main project file*/

#define ANIMATION_IMPL
#include "animation.ino"
#define CUSTOM_CHARS_IMPL
#include "custom_chars.ino"
#define SCENE_IMPL
#include "scene.ino"
#define JOYSTICK_IMPL
#include "joystick.ino"
#define BUZZER_IMPL
#include "buzzer.ino"
#define SCREEN_IMPL
#include "screen.ino"
#define TITLE_IMPL
#include "title.ino"
#define GAME_IMPL
#include "game.ino"
#define SCORE_IMPL
#include "score.ino"
#define TEAM_IMPL
#include "team.ino"
#define IOT_SCORE_IMPL
#include "iot_score.ino"
#define IR_REMOTE_IMPL
#include "ir_remote.ino"

/*
 * project notes:
 *  - use uint32_t for time related values
 */

const int POWER_BUTTON_PIN = 14;
const int LED_PIN = 26;

/* setup and loop */

void setup() {
    Serial.begin(115200);

    /* initialize the lcd */
    lcd.init();
    lcd.clear();

    /* initialize inputs */
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);

    /* initialize the 7 segment display */
    pinMode(STCP_PIN, OUTPUT);
    pinMode(SHCP_PIN, OUTPUT);
    pinMode(DS_PIN, OUTPUT);

    /* initialize power button and led */

    pinMode(POWER_BUTTON_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    /* intialize buzzer */

    pinMode(ACTIVE_BUZZER_PIN, OUTPUT);                                  // Set as output
    ledcAttach(PASSIVE_BUZZER_PIN, BUZZER_FREQUENCY, BUZZER_RESOLUTION); // Set up the PWM pin

    /* other setup */

    digitalWrite(LED_PIN, LOW);

    lcd.noDisplay();
    lcd.noBacklight();

    init_iot_score();
    init_ir_remote();

    init_lcd_mem_slot();
    clear_screen();
    draw_screen();
    to_title_scene();
    draw_screen();
}

int system_on = 0;

void update_power_button() {
    static int last_button_state = LOW;
    bool button_state = digitalRead(POWER_BUTTON_PIN);

    if (button_state == HIGH && last_button_state == LOW) {
        system_on = system_on ^ 0x1;

        if (system_on == 1) {
            lcd.display();
            lcd.backlight();
            digitalWrite(LED_PIN, HIGH);
            reset_sound();
            to_title_scene();
        } else {
            lcd.noDisplay();
            lcd.noBacklight();
            digitalWrite(LED_PIN, LOW);
        }
    }

    last_button_state = button_state;
}

void loop() {
    static uint32_t last_frame_ms = 0;
    uint32_t time_diff = millis() - last_frame_ms;
    last_frame_ms = millis();

    update_power_button();

    if (system_on == 0) {
        delay(33);
        return;
    }

    update_iot_score(time_diff);
    update_sound(time_diff);

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
