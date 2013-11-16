#include "minigame.h"
  
void
minigame_event_constants_init(mrb_state *mrb, struct RClass *c)
{
  mrb_define_const(mrb, c, "JOYSTICK_AXIS", mrb_fixnum_value(ALLEGRO_EVENT_JOYSTICK_AXIS));
  mrb_define_const(mrb, c, "JOYSTICK_BUTTON_DOWN", mrb_fixnum_value(ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN));
  mrb_define_const(mrb, c, "JOYSTICK_BUTTON_UP", mrb_fixnum_value(ALLEGRO_EVENT_JOYSTICK_BUTTON_UP));
  mrb_define_const(mrb, c, "JOYSTICK_CONFIGURATION", mrb_fixnum_value(ALLEGRO_EVENT_JOYSTICK_CONFIGURATION));

  mrb_define_const(mrb, c, "KEY_DOWN", mrb_fixnum_value(ALLEGRO_EVENT_KEY_DOWN));
  mrb_define_const(mrb, c, "KEY_CHAR", mrb_fixnum_value(ALLEGRO_EVENT_KEY_CHAR));
  mrb_define_const(mrb, c, "KEY_UP", mrb_fixnum_value(ALLEGRO_EVENT_KEY_UP));

  mrb_define_const(mrb, c, "MOUSE_AXES", mrb_fixnum_value(ALLEGRO_EVENT_MOUSE_AXES));
  mrb_define_const(mrb, c, "MOUSE_BUTTON_DOWN", mrb_fixnum_value(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN));
  mrb_define_const(mrb, c, "MOUSE_BUTTON_UP", mrb_fixnum_value(ALLEGRO_EVENT_MOUSE_BUTTON_UP));
  mrb_define_const(mrb, c, "MOUSE_ENTER_DISPLAY", mrb_fixnum_value(ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY));
  mrb_define_const(mrb, c, "MOUSE_LEAVE_DISPLAY", mrb_fixnum_value(ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY));
  mrb_define_const(mrb, c, "MOUSE_WARPED", mrb_fixnum_value(ALLEGRO_EVENT_MOUSE_WARPED));

  mrb_define_const(mrb, c, "TIMER", mrb_fixnum_value(ALLEGRO_EVENT_TIMER));

  mrb_define_const(mrb, c, "DISPLAY_EXPOSE", mrb_fixnum_value(ALLEGRO_EVENT_DISPLAY_EXPOSE));
  mrb_define_const(mrb, c, "DISPLAY_RESIZE", mrb_fixnum_value(ALLEGRO_EVENT_DISPLAY_RESIZE));
  mrb_define_const(mrb, c, "DISPLAY_CLOSE", mrb_fixnum_value(ALLEGRO_EVENT_DISPLAY_CLOSE));
  mrb_define_const(mrb, c, "DISPLAY_LOST", mrb_fixnum_value(ALLEGRO_EVENT_DISPLAY_LOST));
  mrb_define_const(mrb, c, "DISPLAY_SWITCH_IN", mrb_fixnum_value(ALLEGRO_EVENT_DISPLAY_SWITCH_IN));
  mrb_define_const(mrb, c, "DISPLAY_SWITCH_OUT", mrb_fixnum_value(ALLEGRO_EVENT_DISPLAY_SWITCH_OUT));
  mrb_define_const(mrb, c, "DISPLAY_ORIENTATION", mrb_fixnum_value(ALLEGRO_EVENT_DISPLAY_ORIENTATION));
}
