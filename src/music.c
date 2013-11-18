#include "minigame.h"

static struct RClass *music_cls = NULL;

static ALLEGRO_MIXER *music_mixer = NULL;
static float musci_volume = 1.0f;

struct MinigameMusic {
  ALLEGRO_AUDIO_STREAM *stream;
  ALLEGRO_MIXER *mixer;
};

static void
music_free(mrb_state *mrb, void *p)
{
  if (p) {
    struct MinigameMusic *music = (struct MinigameMusic*)p;
    if (music->stream)
      al_destroy_audio_stream(music->stream);
  }
}

static mrb_data_type music_t = {"Music", music_free};

static mrb_value
music_load(mrb_state *mrb, mrb_value self)
{
  char *filepath;
  
  struct MinigameMusic *music;

  mrb_get_args(mrb, "z", &filepath);

  music = mrb_malloc(mrb, sizeof(struct MinigameMusic));

  music->stream = al_load_audio_stream(filepath, 4, 1024*2);

  if (music->stream) {
    music->mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
    al_attach_audio_stream_to_mixer(music->stream, music->mixer);
    al_attach_mixer_to_mixer(music->mixer, music_mixer);
    al_set_audio_stream_playing(music->stream, false);
    return mrb_obj_value(mrb_data_object_alloc(mrb, music_cls, music, &music_t));
  }

  return mrb_nil_value();
}

static mrb_value
music_mixer_play(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_mixer_playing(music_mixer, true));
}

static mrb_value
music_play(mrb_state *mrb, mrb_value self)
{
  mrb_bool loop = false;

  struct MinigameMusic *music;

  mrb_get_args(mrb, "|b", &loop);

  music = (struct MinigameMusic*)DATA_PTR(self);

  if (loop)
    al_set_audio_stream_playmode(music->stream, ALLEGRO_PLAYMODE_LOOP);

  return mrb_bool_value(al_set_audio_stream_playing(music->stream, true));
}

void
minigame_music_init(mrb_state *mrb, struct RClass *parent)
{
  music_cls = mrb_define_class_under(mrb, parent, "Music", mrb->object_class);
  MRB_SET_INSTANCE_TT(music_cls, MRB_TT_DATA);

  music_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
  al_attach_mixer_to_mixer(music_mixer, minigame_get_audio_mixer());

  mrb_define_class_method(mrb, music_cls, "load", music_load, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, music_cls, "play", music_mixer_play, MRB_ARGS_NONE());

  mrb_define_method(mrb, music_cls, "play", music_play, MRB_ARGS_OPT(1));
}
