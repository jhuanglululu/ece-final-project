#ifndef ANIMATION_INO
#define ANIMATION_INO

/* this file includes animation logics */

struct Animation {
    uint32_t timer;
    uint32_t frame_ms;
    int frame;
    int frame_count;
    int repeat;
};

void update_animation(Animation *anim, uint32_t time_diff);
void reset_animation(Animation *anim);

#endif
#ifdef ANIMATION_IMPL
#ifndef ANIMATION_IMPL_GUARD
#define ANIMATION_IMPL_GUARD

void update_animation(Animation *anim, uint32_t time_diff) {
    if (anim->repeat == 0 && anim->frame >= anim->frame_count - 1) {
        // return if animation does not need to be repeated
        anim->frame = anim->frame_count - 1;
        return;
    }

    // increment timer and update frame
    anim->timer += time_diff;
    anim->frame += anim->timer / anim->frame_ms;
    anim->timer %= anim->frame_ms;
    anim->frame %= anim->frame_count;
}

void reset_animation(Animation *anim) {
    anim->timer = 0;
    anim->frame = 0;
}
#endif
#endif