#ifndef MRUBY_MINIGAME_H
#define MRUBY_MINIGAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

#include <mruby.h>
#include <mruby/data.h>
#include <mruby/class.h>

extern struct RClass *g_minigame_color_cls;
extern mrb_data_type g_minigame_color_t;

extern void minigame_color_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_display_init(mrb_state *mrb, struct RClass *parent);
extern void minigame_display_final(void);

extern void minigame_register_event_source(ALLEGRO_EVENT_SOURCE *src);
extern struct RClass* minigame_event_init(mrb_state *mrb, struct RClass *parent);
extern void minigame_event_final(void);

extern void minigame_event_constants_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_event_timer_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_key_constants_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_image_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_graphics_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_audio_init(mrb_state *mrb, struct RClass *parent);
extern void minigame_audio_final(void);

extern ALLEGRO_MIXER *minigame_get_audio_mixer(void);

extern void minigame_music_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_sound_init(mrb_state *mrb, struct RClass *parent);

#define MINIGAME_RGB_CLAMP(x) \
if (x < 0) x = 0; \
else if (x > 255) x = 255;

#define MINIGAME_EXPAND_COLOR_ARRAY(colary, dstcolor) \
mrb_int colr, colg, colb, cola = 255; \
mrb_int colary_len; \
colary_len = RARRAY_LEN(colary); \
if (colary_len < 3) mrb_raisef(mrb, E_ARGUMENT_ERROR, "require 3 or 4 array length"); \
colr = mrb_fixnum(mrb_Integer(mrb, RARRAY_PTR(colary)[0])); \
MINIGAME_RGB_CLAMP(colr); \
colg = mrb_fixnum(mrb_Integer(mrb, RARRAY_PTR(colary)[1])); \
MINIGAME_RGB_CLAMP(colg); \
colb = mrb_fixnum(mrb_Integer(mrb, RARRAY_PTR(colary)[2])); \
MINIGAME_RGB_CLAMP(colb); \
if (colary_len > 3) { \
  cola = mrb_fixnum(mrb_Integer(mrb, RARRAY_PTR(colary)[3])); \
  MINIGAME_RGB_CLAMP(cola); \
} \
dstcolor = al_map_rgba(colr, colg, colb, cola); \

#endif /* MRUBY_MINIGAME_H */
