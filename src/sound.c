#include "minigame.h"

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "mruby/array.h"
#include "mruby/hash.h"

static struct RClass *sound_cls = NULL;
static ALLEGRO_MIXER *sound_mixer = NULL;

static mrb_sym sym_loop;
static mrb_sym sym_volume;

struct MinigameSound {
  ALLEGRO_MIXER *mixer;
  ALLEGRO_SAMPLE *sample;
  ALLEGRO_SAMPLE_INSTANCE **instance;
  int idx;
  int len;
};

void
sound_free(mrb_state *mrb, void *p)
{
  if (p) {
    int i;

    struct MinigameSound *sound = (struct MinigameSound*)p;

    al_destroy_mixer(sound->mixer);

    for (i = 0; i < sound->len; ++i) {
      if (sound->instance[i]) al_destroy_sample_instance(sound->instance[i]);
    }

    mrb_free(mrb, sound->instance);

    if (sound->sample) al_destroy_sample(sound->sample);

    mrb_free(mrb, p);
  }
}

static mrb_data_type sound_t = {"Sound", sound_free};

static mrb_value
sound_load(mrb_state *mrb, mrb_value self)
{
  char *filepath;
  mrb_int reserve_size = 8;

  struct MinigameSound *sound;

  mrb_get_args(mrb, "z|i", &filepath, &reserve_size);

  if (reserve_size <= 0) reserve_size = 1;

  sound = mrb_malloc(mrb, sizeof(struct MinigameSound));
  sound->instance = mrb_malloc(mrb, sizeof(sound->instance) * reserve_size);
  sound->idx = 0;
  sound->len = reserve_size;

  sound->sample = al_load_sample(filepath);

  if (sound->sample) {
    int i;

    sound->mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
    al_attach_mixer_to_mixer(sound->mixer, sound_mixer);

    for (i = 0; i < sound->len; ++i) {
      sound->instance[i] = al_create_sample_instance(sound->sample);
      al_attach_sample_instance_to_mixer(sound->instance[i], sound->mixer);
      al_set_sample_instance_playing(sound->instance[i], false);
    }

    return mrb_obj_value(mrb_data_object_alloc(mrb, sound_cls, sound, &sound_t));
  }

  return mrb_nil_value();
}

#define SOUND_VOLUME_CLAMP(x) \
if (x < 0) x = 0; \
else if (x > 100) x = 100;

static mrb_value
sound_mixer_set_volume(mrb_state *mrb, mrb_value self)
{
  mrb_int vol;

  mrb_get_args(mrb, "i", &vol);

  SOUND_VOLUME_CLAMP(vol);

  al_set_mixer_gain(sound_mixer, vol / 100.0);

  return mrb_fixnum_value(al_get_mixer_gain(sound_mixer) * 100);
}

static mrb_value
sound_mixer_get_volume(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(al_get_mixer_gain(sound_mixer) * 100);
}

static mrb_value
sound_play(mrb_state *mrb, mrb_value self)
{
  mrb_value loop;
  mrb_value volume;
  mrb_value opt;
  int argc;
  int i;

  ALLEGRO_PLAYMODE play_mode = ALLEGRO_PLAYMODE_ONCE;
  float play_volume = 1.0f;

  struct MinigameSound *sound;

  argc = mrb_get_args(mrb, "|H", &opt);

  sound = (struct MinigameSound*)DATA_PTR(self);

  if (argc > 0) {
    int i;
    mrb_value keys;

    keys = mrb_funcall(mrb, opt, "keys", 0);

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
        SOUND_VOLUME_CLAMP(vol);
        play_volume = vol / 100.0;
      }
      else
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", k);
    }
  }

  for (i = 0; i < sound->len; ++i) {
    ALLEGRO_SAMPLE_INSTANCE *si = sound->instance[sound->idx];
    if (al_get_sample_instance_playing(si)) {
      sound->idx += 1;
      if (sound->idx >= sound->len) sound->idx = 0;

      continue;
    }
    else {
      sound->idx += 1;
      if (sound->idx >= sound->len) sound->idx = 0;

      al_set_sample_instance_playmode(si, play_mode);
      al_set_sample_instance_gain(si, play_volume);
      return mrb_bool_value(al_set_sample_instance_playing(si, true));
    }
  }

  return mrb_false_value();
}

static mrb_value
sound_set_volume(mrb_state *mrb, mrb_value self)
{
  mrb_int vol;
  
  struct MinigameSound *sound;

  mrb_get_args(mrb, "i", &vol);

  SOUND_VOLUME_CLAMP(vol);

  sound = (struct MinigameSound*)DATA_PTR(self);

  al_set_mixer_gain(sound->mixer, vol / 100.0);

  return mrb_fixnum_value(al_get_mixer_gain(sound->mixer) * 100);
}

static mrb_value
sound_get_volume(mrb_state *mrb, mrb_value self)
{
  struct MinigameSound *sound;

  sound = (struct MinigameSound*)DATA_PTR(self);

  return mrb_fixnum_value(al_get_mixer_gain(sound->mixer) * 100);
}

void
minigame_sound_init(mrb_state *mrb, struct RClass *parent)
{
  sound_cls = mrb_define_class_under(mrb, parent, "Sound", mrb->object_class);
  MRB_SET_INSTANCE_TT(sound_cls, MRB_TT_DATA);

  sound_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
  al_attach_mixer_to_mixer(sound_mixer, minigame_get_audio_mixer());

  mrb_define_class_method(mrb, sound_cls, "load", sound_load, MRB_ARGS_ARG(1, 1));
  mrb_define_class_method(mrb, sound_cls, "volume=", sound_mixer_set_volume, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, sound_cls, "volume", sound_mixer_get_volume, MRB_ARGS_NONE());

  mrb_define_method(mrb, sound_cls, "play", sound_play, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, sound_cls, "volume=", sound_set_volume, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, sound_cls, "volume", sound_get_volume, MRB_ARGS_NONE());

  sym_loop = mrb_intern_lit(mrb, "loop");
  sym_volume = mrb_intern_lit(mrb, "volume");
}
