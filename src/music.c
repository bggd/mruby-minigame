#include "minigame.h"

#include "mruby/array.h"
#include "mruby/hash.h"

static struct RClass *music_cls = NULL;

static ALLEGRO_MIXER *music_mixer = NULL;

static mrb_sym sym_loop;
static mrb_sym sym_volume;

struct MinigameMusic {
  ALLEGRO_AUDIO_STREAM *stream;
  ALLEGRO_MIXER *mixer;
  float volume;
};

static void
music_free(mrb_state *mrb, void *p)
{
  if (p) {
    struct MinigameMusic *music = (struct MinigameMusic*)p;
    if (music->stream)
      al_destroy_audio_stream(music->stream);

    mrb_free(mrb, p);
  }
}

static mrb_data_type music_t = {"Music", music_free};

#define MUSIC_VOLUME_CLAMP(x) \
if (x < 0) x = 0; \
else if (x > 100) x = 100;

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
music_mixer_set_volume(mrb_state *mrb, mrb_value self)
{
  mrb_int vol;

  mrb_get_args(mrb, "i", &vol);

  MUSIC_VOLUME_CLAMP(vol);

  al_set_mixer_gain(music_mixer, vol / 100.0);
  
  return mrb_fixnum_value(al_get_mixer_gain(music_mixer) * 100);
}

static mrb_value
music_mixer_get_volume(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(al_get_mixer_gain(music_mixer) * 100);
}

static mrb_value
music_play(mrb_state *mrb, mrb_value self)
{
  mrb_value loop;
  mrb_value volume;
  mrb_value opt;
  int argc;

  ALLEGRO_PLAYMODE play_mode = ALLEGRO_PLAYMODE_ONCE;
  float play_volume = 1.0f;

  struct MinigameMusic *music;

  argc = mrb_get_args(mrb, "|H", &opt);

  music = (struct MinigameMusic*)DATA_PTR(self);

  if (argc > 0) {
    int i;
    mrb_value keys;

    keys = mrb_hash_keys(mrb, opt);

    for (i = 0; i < RARRAY_LEN(keys); ++i) {
      mrb_sym sym;
      mrb_value k = RARRAY_PTR(keys)[i];

      if (mrb_string_p(k))
        sym = mrb_intern_str(mrb, k);
      else
        sym = mrb_symbol(k);

      if (sym == sym_loop) {
        loop = mrb_hash_get(mrb, opt, k);
        if (mrb_bool(loop))
          play_mode = ALLEGRO_PLAYMODE_LOOP;
      }
      else if (sym == sym_volume) {
        int vol;
        volume = mrb_hash_get(mrb, opt, k);
        vol = mrb_fixnum(mrb_Integer(mrb, volume));
        MUSIC_VOLUME_CLAMP(vol);
        play_volume = vol / 100.0;
      }
      else
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", k);
    }
  }

  al_set_audio_stream_playmode(music->stream, play_mode);

  al_set_audio_stream_gain(music->stream, play_volume);

  return mrb_bool_value(al_set_audio_stream_playing(music->stream, true));
}

static mrb_value
music_stop(mrb_state *mrb, mrb_value self)
{
  struct MinigameMusic *music;

  music = (struct MinigameMusic*)DATA_PTR(self);

  return mrb_bool_value(al_set_audio_stream_playing(music->stream, false));
}

static mrb_value
music_seek(mrb_state *mrb, mrb_value self)
{
  mrb_float time;

  struct MinigameMusic *music;

  mrb_get_args(mrb, "f", &time);

  music = (struct MinigameMusic*)DATA_PTR(self);

  return mrb_bool_value(al_seek_audio_stream_secs(music->stream, time));
}

static mrb_value
music_set_volume(mrb_state *mrb, mrb_value self)
{
  mrb_int vol;

  struct MinigameMusic *music;

  mrb_get_args(mrb, "i", &vol);

  MUSIC_VOLUME_CLAMP(vol)

  music = (struct MinigameMusic*)DATA_PTR(self);

  al_set_audio_stream_gain(music->stream, vol / 100.0);

  return mrb_fixnum_value(al_get_audio_stream_gain(music->stream) * 100);
}

static mrb_value
music_get_volume(mrb_state *mrb, mrb_value self)
{
  struct MinigameMusic *music;

  music = (struct MinigameMusic*)DATA_PTR(self);

  return mrb_fixnum_value(al_get_audio_stream_gain(music->stream) * 100);
}

static mrb_value
music_get_length(mrb_state *mrb, mrb_value self)
{
  struct MinigameMusic *music;

  music = (struct MinigameMusic*)DATA_PTR(self);

  return mrb_float_value(mrb, al_get_audio_stream_length_secs(music->stream));
}

void
minigame_music_init(mrb_state *mrb, struct RClass *parent)
{
  music_cls = mrb_define_class_under(mrb, parent, "Music", mrb->object_class);
  MRB_SET_INSTANCE_TT(music_cls, MRB_TT_DATA);

  music_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
  al_attach_mixer_to_mixer(music_mixer, minigame_get_audio_mixer());

  mrb_define_class_method(mrb, music_cls, "load", music_load, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, music_cls, "volume=", music_mixer_set_volume, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, music_cls, "volume", music_mixer_get_volume, MRB_ARGS_NONE());

  mrb_define_method(mrb, music_cls, "play", music_play, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, music_cls, "stop", music_stop, MRB_ARGS_NONE());
  mrb_define_method(mrb, music_cls, "seek", music_seek, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, music_cls, "volume=", music_set_volume, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, music_cls, "volume", music_get_volume, MRB_ARGS_NONE());
  mrb_define_method(mrb, music_cls, "length", music_get_length, MRB_ARGS_NONE());

  sym_loop = mrb_intern_lit(mrb, "loop");
  sym_volume = mrb_intern_lit(mrb, "volume");
}
