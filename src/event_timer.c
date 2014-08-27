#include "minigame.h"

static struct RClass *event_timer_cls = NULL;

static void
event_timer_free(mrb_state *mrb, void *p)
{
  if (p) {
    ALLEGRO_TIMER *timer = (ALLEGRO_TIMER*)p;
    al_destroy_timer(timer);
  }
}

static mrb_data_type event_timer_t = {"Timer", event_timer_free};

static mrb_value
event_timer_new(mrb_state *mrb, mrb_value self)
{
  mrb_float speed_secs;

  ALLEGRO_TIMER *timer;

  mrb_get_args(mrb, "f", &speed_secs);

  timer = al_create_timer(speed_secs);

  if (timer) {
    minigame_register_event_source(al_get_timer_event_source(timer));
    mrb_data_init(self, timer, &event_timer_t);
    return self;
  }
  
  return mrb_nil_value();
}

static mrb_value
event_timer_start(mrb_state *mrb, mrb_value self)
{
  ALLEGRO_TIMER *timer;

  timer = (ALLEGRO_TIMER*)DATA_PTR(self);

  al_start_timer(timer);

  return mrb_nil_value();
}

void
minigame_event_timer_init(mrb_state *mrb, struct RClass *parent)
{
  event_timer_cls = mrb_define_class_under(mrb, parent, "Timer", mrb->object_class);
  MRB_SET_INSTANCE_TT(event_timer_cls, MRB_TT_DATA);

  mrb_define_method(mrb, event_timer_cls, "initialize", event_timer_new, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, event_timer_cls, "start", event_timer_start, MRB_ARGS_NONE());
}
