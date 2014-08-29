#include "minigame.h"

static mrb_state *first_mrb = NULL;

static mrb_value
minigame_get_time(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, al_get_time());
}

static mrb_value
minigame_rest(mrb_state *mrb, mrb_value self)
{
  mrb_float sec;

  mrb_get_args(mrb, "f", &sec);

  al_rest(sec);

  return mrb_nil_value();
}

void
mrb_mruby_minigame_gem_init(mrb_state *mrb)
{
  struct RClass *c;

  al_init();

  c = mrb_define_module(mrb, "Minigame");

  if (first_mrb == NULL) first_mrb = mrb;
  if (first_mrb != mrb) return;

  mrb_define_module_function(mrb, c, "get_time", minigame_get_time, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, c, "rest", minigame_rest, MRB_ARGS_REQ(1));

  minigame_color_init(mrb, c);
  minigame_display_init(mrb, c);
  minigame_event_constants_init(mrb, minigame_event_init(mrb, c));
  minigame_key_constants_init(mrb, c);
  minigame_image_init(mrb, c);
  minigame_graphics_init(mrb, c);
  minigame_audio_init(mrb, c);
  minigame_music_init(mrb, c);
  minigame_sound_init(mrb, c);
}

void
mrb_mruby_minigame_gem_final(mrb_state *mrb)
{
  if (first_mrb != mrb) return;

  minigame_audio_final();
  minigame_display_final();
  minigame_event_final();
}
