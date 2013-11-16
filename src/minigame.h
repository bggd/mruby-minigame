#ifndef MRUBY_MINIGAME_H
#define MRUBY_MINIGAME_H

#include <allegro5/allegro.h>

#include <mruby.h>
#include <mruby/data.h>
#include <mruby/class.h>

extern struct RClass *g_minigame_color_cls;
extern mrb_data_type g_minigame_color_t;

extern void minigame_color_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_display_init(mrb_state *mrb, struct RClass *parent);
extern void minigame_display_final(void);

extern void minigame_event_register_display(ALLEGRO_DISPLAY *disp);
extern struct RClass* minigame_event_init(mrb_state *mrb, struct RClass *parent);
extern void minigame_event_final(void);

extern void minigame_event_constants_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_key_constants_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_image_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_graphics_init(mrb_state *mrb, struct RClass *parent);

extern void minigame_font_init(mrb_state *mrb, struct RClass *parent);

#endif /* MRUBY_MINIGAME_H */
