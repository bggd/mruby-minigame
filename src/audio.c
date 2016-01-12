#include "minigame.h"

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

static ALLEGRO_VOICE *audio_voice = NULL;
ALLEGRO_MIXER *g_audio_mixer;

static mrb_value
audio_suspend(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_mixer_playing(g_audio_mixer, false));
}

static mrb_value
audio_resume(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_mixer_playing(g_audio_mixer, true));
}

static mrb_value
audio_set_volume(mrb_state *mrb, mrb_value self)
{
  mrb_int vol;

  mrb_get_args(mrb, "i", &vol);

  al_set_mixer_gain(g_audio_mixer, vol);

  return mrb_fixnum_value(al_get_mixer_gain(g_audio_mixer));
}

static mrb_value
audio_get_volume(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(al_get_mixer_gain(g_audio_mixer));
}

void
minigame_audio_init(mrb_state *mrb, struct RClass *parent)
{
  struct RClass *c;

  al_install_audio();
  al_init_acodec_addon();

  audio_voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
  g_audio_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);

  al_attach_mixer_to_voice(g_audio_mixer, audio_voice);

  c = mrb_define_module_under(mrb, parent, "Audio");

  mrb_define_class_method(mrb, c, "suspend", audio_suspend, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, c, "resume", audio_resume, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, c, "volume=", audio_set_volume, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, c, "volume", audio_get_volume, MRB_ARGS_NONE());
}

void
minigame_audio_final(void)
{
  if (audio_voice)
    al_destroy_voice(audio_voice);
}

