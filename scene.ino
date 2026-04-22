#ifndef SCENE_INO
#define SCENE_INO

enum class scene_e { Title, Score, Team, Game };

extern scene_e current_scene;

#endif
#ifdef SCENE_IMPL
#ifndef SCENE_IMPL_GUARD
#define SCENE_IMPL_GUARD

scene_e current_scene = scene_e::Title;

#endif
#endif
