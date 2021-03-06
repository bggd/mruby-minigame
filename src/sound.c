#include "minigame.h"

#include <allegro5/allegro_acodec.h>

#include "mruby/variable.h"

static struct RClass *sound_buffer_cls = NULL;
static struct RClass *sound_cls = NULL;

static ALLEGRO_MIXER *sound_mixer = NULL;

static mrb_sym sym_sound_buffer;

static mrb_value
sound_suspend(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_mixer_playing(sound_mixer, false));
}

static mrb_value
sound_resume(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_mixer_playing(sound_mixer, true));
}

static mrb_value
sound_set_master_volume(mrb_state *mrb, mrb_value self)
{
  mrb_float vol;

  mrb_get_args(mrb, "f", &vol);

  if (vol < 0) vol = 0;
  al_set_mixer_gain(sound_mixer, vol);

  return mrb_float_value(mrb, al_get_mixer_gain(sound_mixer));
}

static mrb_value
sound_get_master_volume(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, al_get_mixer_gain(sound_mixer));
}

static void
sound_buffer_free(mrb_state *mrb, void *p)
{
  if (p) al_destroy_sample((ALLEGRO_SAMPLE*)p);
}

static mrb_data_type sound_buffer_t = {"SoundBuffer", sound_buffer_free};

static mrb_value
sound_buffer_load(mrb_state *mrb, mrb_value self)
{
  const char *filepath;
  ALLEGRO_SAMPLE *sample;

  mrb_get_args(mrb, "z", &filepath);

  sample = al_load_sample(filepath);
  if (sample)
    return mrb_obj_value(mrb_data_object_alloc(mrb, sound_buffer_cls, sample, &sound_buffer_t));
  else
    mrb_raisef(mrb, E_RUNTIME_ERROR, "Could not load '%S'", mrb_str_new_cstr(mrb, filepath));
}

static void
sound_free(mrb_state *mrb, void *p)
{
  if (p) al_destroy_sample_instance((ALLEGRO_SAMPLE_INSTANCE*)p);
}

static mrb_data_type sound_t = {"Sound", sound_free};

static mrb_value
sound_new(mrb_state *mrb, mrb_value self)
{
  mrb_value buf;
  ALLEGRO_SAMPLE_INSTANCE *si;

  mrb_get_args(mrb, "o", &buf);

  mrb_data_check_type(mrb, buf, &sound_buffer_t);

  si = al_create_sample_instance((ALLEGRO_SAMPLE*)DATA_PTR(buf));
  if (si) {
    al_attach_sample_instance_to_mixer(si, sound_mixer);
    mrb_data_init(self, si, &sound_t);
    mrb_iv_set(mrb, self, sym_sound_buffer, buf);
    return self;
  }
  else
    mrb_raise(mrb, E_RUNTIME_ERROR, "Could not create Sound");
}

static mrb_value
sound_play(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_play_sample_instance((ALLEGRO_SAMPLE_INSTANCE*)DATA_PTR(self)));
}

static mrb_value
sound_rewind(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_set_sample_instance_position((ALLEGRO_SAMPLE_INSTANCE*)DATA_PTR(self), 0));
}

void
minigame_sound_init(mrb_state *mrb, struct RClass *parent)
{
  sound_mixer = al_create_mixer(al_get_mixer_frequency(g_audio_mixer), al_get_mixer_depth(g_audio_mixer), al_get_mixer_channels(g_audio_mixer));
  al_attach_mixer_to_mixer(sound_mixer, g_audio_mixer);

  sound_buffer_cls = mrb_define_class_under(mrb, parent, "SoundBuffer", mrb->object_class);
  MRB_SET_INSTANCE_TT(sound_buffer_cls, MRB_TT_DATA);

  mrb_define_class_method(mrb, sound_buffer_cls, "load", sound_buffer_load, MRB_ARGS_REQ(1));

  mrb_undef_method(mrb, sound_buffer_cls, "initialize");

  sound_cls = mrb_define_class_under(mrb, parent, "Sound", mrb->object_class);
  MRB_SET_INSTANCE_TT(sound_cls, MRB_TT_DATA);

  mrb_define_class_method(mrb, sound_cls, "volume=", sound_set_master_volume, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, sound_cls, "volume", sound_get_master_volume, MRB_ARGS_NONE());
  
  mrb_define_class_method(mrb, sound_cls, "suspend", sound_suspend, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, sound_cls, "resume", sound_resume, MRB_ARGS_NONE());

  mrb_define_method(mrb, sound_cls, "initialize", sound_new, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, sound_cls, "play", sound_play, MRB_ARGS_NONE());
  mrb_define_method(mrb, sound_cls, "rewind", sound_rewind, MRB_ARGS_NONE());

  sym_sound_buffer = mrb_intern_lit(mrb, "@sound_buffer");
}

