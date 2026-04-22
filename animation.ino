#ifndef ANIMATION_INO
#define ANIMATION_INO
// animation helper
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
    if (anim->repeat == 0 && anim->frame == anim->frame_count - 1)
        return;
    else if (anim->repeat == 0 && anim->frame >= anim->frame_count)
        anim->frame = anim->frame_count - 1;

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