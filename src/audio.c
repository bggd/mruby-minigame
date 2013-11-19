#include "minigame.h"

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

static ALLEGRO_VOICE *audio_voice = NULL;
static ALLEGRO_MIXER *audio_mixer = NULL;

ALLEGRO_MIXER*
minigame_get_audio_mixer(void)
{
  return audio_mixer;
}

#define AUDIO_VOLUME_CLAMP(x) \
if (x < 0) x = 0; \
else if (x > 100) x = 100;

static mrb_value
audio_set_volume(mrb_state *mrb, mrb_value self)
{
  mrb_int vol;

  mrb_get_args(mrb, "i", &vol);

  AUDIO_VOLUME_CLAMP(vol);

  al_set_mixer_gain(audio_mixer, vol / 100.0);

  return mrb_fixnum_value(al_get_mixer_gain(audio_mixer) * 100);
}

static mrb_value
audio_get_volume(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(al_get_mixer_gain(audio_mixer) * 100);
}

void
minigame_audio_init(mrb_state *mrb, struct RClass *parent)
{
  struct RClass *c;

  al_install_audio();
  al_init_acodec_addon();

  audio_voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
  audio_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);

  al_attach_mixer_to_voice(audio_mixer, audio_voice);

  c = mrb_define_module_under(mrb, parent, "Audio");

  mrb_define_class_method(mrb, c, "volume=", audio_set_volume, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, c, "volume", audio_get_volume, MRB_ARGS_NONE());
}

void
minigame_audio_final(void)
{
  if (audio_voice)
    al_destroy_voice(audio_voice);
}
