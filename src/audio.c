#include "minigame.h"

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

static ALLEGRO_VOICE *audio_voice = NULL;
static ALLEGRO_MIXER *audio_mixer = NULL;

static float audio_volume = 1.0f;

ALLEGRO_MIXER*
minigame_get_audio_mixer(void)
{
  return audio_mixer;
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
}

void
minigame_audio_final(void)
{
  if (audio_voice)
    al_destroy_voice(audio_voice);
}
