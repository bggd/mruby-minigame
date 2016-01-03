#include "minigame.h"

#include "mruby/variable.h"

static struct RClass *event_cls;
static ALLEGRO_EVENT_QUEUE *queue = NULL;

static mrb_sym ev_type;
static mrb_sym ev_keyboard;
static mrb_sym ev_keyboard_keycode;
static mrb_sym ev_mouse;
static mrb_sym ev_mouse_x;
static mrb_sym ev_mouse_y;
static mrb_sym ev_mouse_z;
static mrb_sym ev_mouse_w;
static mrb_sym ev_mouse_dx;
static mrb_sym ev_mouse_dy;
static mrb_sym ev_mouse_dz;
static mrb_sym ev_mouse_dw;
static mrb_sym ev_mouse_button;

void
minigame_register_event_source(ALLEGRO_EVENT_SOURCE *src)
{
  al_register_event_source(queue, src);
}

void
minigame_unregister_event_source(ALLEGRO_EVENT_SOURCE *src)
{
  al_unregister_event_source(queue, src);
}

static inline void
set_properties(mrb_state *mrb, ALLEGRO_EVENT *ev)
{
  mrb_value kb;
  mrb_value mouse;

  mrb_mod_cv_set(mrb, event_cls, ev_type, mrb_fixnum_value(ev->type));

  switch (ev->type) {
    case ALLEGRO_EVENT_KEY_DOWN:
    case ALLEGRO_EVENT_KEY_UP:
      kb = mrb_mod_cv_get(mrb, event_cls, ev_keyboard);
      mrb_iv_set(mrb, kb, ev_keyboard_keycode, mrb_fixnum_value(ev->keyboard.keycode));
      break;
    case ALLEGRO_EVENT_MOUSE_AXES:
    case ALLEGRO_EVENT_MOUSE_WARPED:
      mouse = mrb_mod_cv_get(mrb, event_cls, ev_mouse);
      mrb_iv_set(mrb, mouse, ev_mouse_x, mrb_fixnum_value(ev->mouse.x));
      mrb_iv_set(mrb, mouse, ev_mouse_y, mrb_fixnum_value(ev->mouse.y));
      mrb_iv_set(mrb, mouse, ev_mouse_z, mrb_fixnum_value(ev->mouse.z));
      mrb_iv_set(mrb, mouse, ev_mouse_w, mrb_fixnum_value(ev->mouse.w));
      mrb_iv_set(mrb, mouse, ev_mouse_dx, mrb_fixnum_value(ev->mouse.dx));
      mrb_iv_set(mrb, mouse, ev_mouse_dy, mrb_fixnum_value(ev->mouse.dy));
      mrb_iv_set(mrb, mouse, ev_mouse_dz, mrb_fixnum_value(ev->mouse.dz));
      mrb_iv_set(mrb, mouse, ev_mouse_dw, mrb_fixnum_value(ev->mouse.dw));
      break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
      mouse = mrb_mod_cv_get(mrb, event_cls, ev_mouse);
      mrb_iv_set(mrb, mouse, ev_mouse_x, mrb_fixnum_value(ev->mouse.x));
      mrb_iv_set(mrb, mouse, ev_mouse_y, mrb_fixnum_value(ev->mouse.y));
      mrb_iv_set(mrb, mouse, ev_mouse_z, mrb_fixnum_value(ev->mouse.z));
      mrb_iv_set(mrb, mouse, ev_mouse_w, mrb_fixnum_value(ev->mouse.w));
      mrb_iv_set(mrb, mouse, ev_mouse_button, mrb_fixnum_value(ev->mouse.button));
      break;
    default:
      break;
  }
}

static mrb_value
event_is_empty(mrb_state *mrb, mrb_value self)
{
  return mrb_bool_value(al_is_event_queue_empty(queue));
}

static mrb_value
event_flush(mrb_state *mrb, mrb_value self)
{
  al_flush_event_queue(queue);

  return mrb_nil_value();
}

static mrb_value
event_wait_for_event(mrb_state *mrb, mrb_value self)
{
  ALLEGRO_EVENT ev;

  al_wait_for_event(queue, &ev);
  set_properties(mrb, &ev);

  return self;
}

static mrb_value
event_get_next_event(mrb_state *mrb, mrb_value self)
{
  ALLEGRO_EVENT ev;

  if (al_get_next_event(queue, &ev)) {
    set_properties(mrb, &ev);
    return self;
  }

  return mrb_nil_value();
}

struct RClass *
minigame_event_init(mrb_state *mrb, struct RClass *parent)
{
  queue = al_create_event_queue();

  if (al_install_keyboard())
    al_register_event_source(queue, al_get_keyboard_event_source());
  if (al_install_mouse())
    al_register_event_source(queue, al_get_mouse_event_source());

  event_cls = mrb_define_module_under(mrb, parent, "Event");

  mrb_define_module_function(mrb, event_cls, "empty?", event_is_empty, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, event_cls, "flush", event_flush, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, event_cls, "_wait_for_event", event_wait_for_event, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, event_cls, "_get_next_event", event_get_next_event, MRB_ARGS_NONE());

  ev_type = mrb_intern_lit(mrb, "@@type");

  ev_keyboard = mrb_intern_lit(mrb, "@@keyboard");
  ev_keyboard_keycode = mrb_intern_lit(mrb, "@keycode");

  ev_mouse = mrb_intern_lit(mrb, "@@mouse");
  ev_mouse_x = mrb_intern_lit(mrb, "@x");
  ev_mouse_y = mrb_intern_lit(mrb, "@y");
  ev_mouse_z = mrb_intern_lit(mrb, "@z");
  ev_mouse_w = mrb_intern_lit(mrb, "@w");
  ev_mouse_dx = mrb_intern_lit(mrb, "@dx");
  ev_mouse_dy = mrb_intern_lit(mrb, "@dy");
  ev_mouse_dz = mrb_intern_lit(mrb, "@dz");
  ev_mouse_dw = mrb_intern_lit(mrb, "@dw");
  ev_mouse_button = mrb_intern_lit(mrb, "@button");

  minigame_event_timer_init(mrb, event_cls);

  return event_cls;
}

void
minigame_event_final(void)
{
  if (queue) al_destroy_event_queue(queue);
}
