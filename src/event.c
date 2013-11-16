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
minigame_event_register_display(ALLEGRO_DISPLAY *disp)
{
  al_register_event_source(queue, al_get_display_event_source(disp));
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
event_poll(mrb_state *mrb, mrb_value self)
{
  mrb_value blk;
  ALLEGRO_EVENT ev;

  mrb_get_args(mrb, "&", &blk);

  if (mrb_nil_p(blk)) al_flush_event_queue(queue);

  while (al_get_next_event(queue, &ev)) {
    set_properties(mrb, &ev);
    mrb_yield(mrb, blk, self);
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

  mrb_define_module_function(mrb, event_cls, "poll", event_poll, MRB_ARGS_BLOCK());

  ev_type = mrb_intern_cstr(mrb, "@@type");

  ev_keyboard = mrb_intern_cstr(mrb, "@@keyboard");
  ev_keyboard_keycode = mrb_intern_cstr(mrb, "@keycode");

  ev_mouse = mrb_intern_cstr(mrb, "@@mouse");
  ev_mouse_x = mrb_intern_cstr(mrb, "@x");
  ev_mouse_y = mrb_intern_cstr(mrb, "@y");
  ev_mouse_z = mrb_intern_cstr(mrb, "@z");
  ev_mouse_w = mrb_intern_cstr(mrb, "@w");
  ev_mouse_dx = mrb_intern_cstr(mrb, "@dx");
  ev_mouse_dy = mrb_intern_cstr(mrb, "@dy");
  ev_mouse_dz = mrb_intern_cstr(mrb, "@dz");
  ev_mouse_dw = mrb_intern_cstr(mrb, "@dw");
  ev_mouse_button = mrb_intern_cstr(mrb, "@button");

  return event_cls;
}

void
minigame_event_final(void)
{
  if (queue) al_destroy_event_queue(queue);
}
