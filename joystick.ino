#ifndef JOYSTICK_INO
#define JOYSTICK_INO

enum class joystick_dir_t { None, Up, Down, Left, Right };

joystick_dir_t get_joystick_dir();

#endif
#ifdef JOYSTICK_IMPL
#ifndef JOYSTICK_IMPL_GUARD
#define JOYSTICK_IMPL_GUARD

const int JOYSTICK_X_PIN = 35;
const int JOYSTICK_Y_PIN = 34;

const int JOYSTICK_OFFSET = 4096 / 2;
const int JOYSTICK_THRESH = 512;

joystick_dir_t get_joystick_dir() {
    int x = analogRead(JOYSTICK_X_PIN) - JOYSTICK_OFFSET;
    int y = analogRead(JOYSTICK_Y_PIN) - JOYSTICK_OFFSET;

    if (abs(x) < JOYSTICK_THRESH && abs(y) < JOYSTICK_THRESH)
        return joystick_dir_t::None;
    if (abs(x) < abs(y) && y < 0)
        return joystick_dir_t::Up;
    else if (abs(x) < abs(y))
        return joystick_dir_t::Down;
    else if (x < 0)
        return joystick_dir_t::Left;
    else
        return joystick_dir_t::Right;
}
#endif
#endif