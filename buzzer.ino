#ifndef BUZZER_INO
#define BUZZER_INO

const int PASSIVE_BUZZER_PIN = 27; // buzzer pin
const int ACTIVE_BUZZER_PIN = 33;  // buzzer pin
const int BUZZER_FREQUENCY = 2000;
const int BUZZER_RESOLUTION = 8;

void reset_sound();
void update_sound(uint32_t time_diff);
void play_eating_sound();

#endif

#ifdef BUZZER_IMPL
#ifndef BUZZER_IMPL_GUARD
#define BUZZER_IMPL_GUARD

const float MUSIC_NOTE[] = {261.63, 329.63, 349.23, 392.00, 349.23, 329.63, 261.63, 329.63, 349.23, 392.00, 349.23,
                            329.63, 261.63, 329.63, 349.23, 392.00, 349.23, 329.63, 261.63, 196.00, 246.94, 261.63};

const int MUSIC_DURATION[] = {500, 500, 500, 500, 250, 250, 500, 500, 500, 500, 250,
                              250, 500, 500, 500, 500, 250, 250, 500, 500, 500, 500};

const int MUSIC_PAUSE[] = {100, 100, 50,  50, 50, 100, 100, 100, 50, 50, 50,
                           100, 100, 100, 50, 50, 50,  100, 100, 50, 50, 100};

const int MUSIC_NOTE_COUNT = 22; // 6 * 3 + 4

int32_t eating_sound_progress = 0;
int32_t music_duration = 0; // for pause and duration
int music_progress = 0;     // music index
int note_playing = 0;       // 1 when playing note and 0 when pausing

void reset_sound() {
    eating_sound_progress = 0;
    music_duration = 0;
    music_progress = 0;
    note_playing = 0;
}

void update_sound(uint32_t time_diff) {
    // can't use delay since it is blocking

    // stop eating sound after 1ms
    if (eating_sound_progress > 0) {

        eating_sound_progress -= time_diff;

        if (eating_sound_progress <= 0) {
            eating_sound_progress = 0;
            digitalWrite(ACTIVE_BUZZER_PIN, LOW);
        }
    }

    music_duration -= time_diff;
    while (music_duration <= 0) {
        if (note_playing == 1) {
            note_playing = 0;

            // pause before next note
            music_duration += MUSIC_PAUSE[music_progress];
            ledcWriteTone(PASSIVE_BUZZER_PIN, 0);
        } else {
            note_playing = 1;

            // move to next note
            music_progress = (music_progress + 1) % MUSIC_NOTE_COUNT;
            ledcWriteTone(PASSIVE_BUZZER_PIN, MUSIC_NOTE[music_progress]);
            music_duration += MUSIC_DURATION[music_progress];
        }
    }
}

void play_eating_sound() {
    eating_sound_progress = 100;
    digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
}

#endif
#endif