#include "minigame.h"

#include "mruby/array.h"
#include "mruby/hash.h"

static struct RClass *music_cls = NULL;

static ALLEGRO_MIXER *music_mixer = NULL;

static void
music_free(mrb_state *mrb, void *p)
{
  if (p) al_destroy_audio_stream((ALLEGRO_AUDIO_STREAM*)p);
}

static mrb_data_type music_t = {"Music", music_free};

static mrb_value
music_suspend(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_mixer_playing(music_mixer, false));
}

static mrb_value
music_resume(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_mixer_playing(music_mixer, true));
}

static mrb_value
music_set_master_volume(mrb_state *mrb, mrb_value self)
{
  mrb_float vol;

  mrb_get_args(mrb, "f", &vol);

  if (vol < 0) vol = 0;
  al_set_mixer_gain(music_mixer, vol);

  return mrb_float_value(mrb, al_get_mixer_gain(music_mixer));
}

static mrb_value
music_get_master_volume(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, al_get_mixer_gain(music_mixer));
}

static mrb_value
music_load(mrb_state *mrb, mrb_value self)
{
  const char *filepath;
  ALLEGRO_AUDIO_STREAM *stream;

  mrb_get_args(mrb, "z", &filepath);

  stream = al_load_audio_stream(filepath, 4, 2048);
  if (stream) {
    al_attach_audio_stream_to_mixer(stream, music_mixer);
    return mrb_obj_value(mrb_data_object_alloc(mrb, music_cls, stream, &music_t));
  }
  else
    mrb_raisef(mrb, E_RUNTIME_ERROR, "Could not load '%S'", mrb_str_new_cstr(mrb, filepath));
}

static mrb_value
music_get_duration(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, al_get_audio_stream_length_secs((ALLEGRO_AUDIO_STREAM*)DATA_PTR(self)));
}

static mrb_value
music_set_volume(mrb_state *mrb, mrb_value self)
{
  mrb_float vol;

  mrb_get_args(mrb, "f", &vol);

  if (vol < 0) vol = 0;
  al_set_audio_stream_gain((ALLEGRO_AUDIO_STREAM*)DATA_PTR(self), vol);

  return mrb_float_value(mrb, al_get_audio_stream_gain((ALLEGRO_AUDIO_STREAM*)DATA_PTR(self)));
}

static mrb_value
music_get_volume(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, al_get_audio_stream_gain((ALLEGRO_AUDIO_STREAM*)DATA_PTR(self)));
}

static mrb_value
music_play(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_audio_stream_playing((ALLEGRO_AUDIO_STREAM*)DATA_PTR(self), true));
}

void
minigame_music_init(mrb_state *mrb, struct RClass *parent)
{
  music_cls = mrb_define_class_under(mrb, parent, "Music", mrb->object_class);
  MRB_SET_INSTANCE_TT(music_cls, MRB_TT_DATA);

  music_mixer = al_create_mixer(al_get_mixer_frequency(g_audio_mixer), al_get_mixer_depth(g_audio_mixer), al_get_mixer_channels(g_audio_mixer));
  al_attach_mixer_to_mixer(music_mixer, g_audio_mixer);

  mrb_define_class_method(mrb, music_cls, "volume=", music_set_master_volume, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, music_cls, "volume", music_get_master_volume, MRB_ARGS_NONE());

  mrb_define_class_method(mrb, music_cls, "load", music_load, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, music_cls, "suspend", music_suspend, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, music_cls, "resume", music_resume, MRB_ARGS_NONE());

  mrb_undef_method(mrb, music_cls, "initialize");

  mrb_define_method(mrb, music_cls, "duration", music_get_duration, MRB_ARGS_NONE());
  mrb_define_method(mrb, music_cls, "volume=", music_set_volume, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, music_cls, "volume", music_get_volume, MRB_ARGS_NONE());

  mrb_define_method(mrb, music_cls, "play", music_play, MRB_ARGS_NONE());
}

