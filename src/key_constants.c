#include "minigame.h"

void
minigame_key_constants_init(mrb_state *mrb, struct RClass *parent)
{
  struct RClass *c = NULL;

  c = mrb_define_module_under(mrb, parent, "Key");

  mrb_define_const(mrb, c, "A", mrb_fixnum_value(ALLEGRO_KEY_A));
  mrb_define_const(mrb, c, "B", mrb_fixnum_value(ALLEGRO_KEY_B));
  mrb_define_const(mrb, c, "C", mrb_fixnum_value(ALLEGRO_KEY_C));
  mrb_define_const(mrb, c, "D", mrb_fixnum_value(ALLEGRO_KEY_D));
  mrb_define_const(mrb, c, "E", mrb_fixnum_value(ALLEGRO_KEY_E));
  mrb_define_const(mrb, c, "F", mrb_fixnum_value(ALLEGRO_KEY_F));
  mrb_define_const(mrb, c, "G", mrb_fixnum_value(ALLEGRO_KEY_G));
  mrb_define_const(mrb, c, "H", mrb_fixnum_value(ALLEGRO_KEY_H));
  mrb_define_const(mrb, c, "I", mrb_fixnum_value(ALLEGRO_KEY_I));
  mrb_define_const(mrb, c, "J", mrb_fixnum_value(ALLEGRO_KEY_J));
  mrb_define_const(mrb, c, "K", mrb_fixnum_value(ALLEGRO_KEY_K));
  mrb_define_const(mrb, c, "L", mrb_fixnum_value(ALLEGRO_KEY_L));
  mrb_define_const(mrb, c, "M", mrb_fixnum_value(ALLEGRO_KEY_M));
  mrb_define_const(mrb, c, "N", mrb_fixnum_value(ALLEGRO_KEY_N));
  mrb_define_const(mrb, c, "O", mrb_fixnum_value(ALLEGRO_KEY_O));
  mrb_define_const(mrb, c, "P", mrb_fixnum_value(ALLEGRO_KEY_P));
  mrb_define_const(mrb, c, "Q", mrb_fixnum_value(ALLEGRO_KEY_Q));
  mrb_define_const(mrb, c, "R", mrb_fixnum_value(ALLEGRO_KEY_R));
  mrb_define_const(mrb, c, "S", mrb_fixnum_value(ALLEGRO_KEY_S));
  mrb_define_const(mrb, c, "T", mrb_fixnum_value(ALLEGRO_KEY_T));
  mrb_define_const(mrb, c, "U", mrb_fixnum_value(ALLEGRO_KEY_U));
  mrb_define_const(mrb, c, "V", mrb_fixnum_value(ALLEGRO_KEY_V));
  mrb_define_const(mrb, c, "W", mrb_fixnum_value(ALLEGRO_KEY_W));
  mrb_define_const(mrb, c, "X", mrb_fixnum_value(ALLEGRO_KEY_X));
  mrb_define_const(mrb, c, "Y", mrb_fixnum_value(ALLEGRO_KEY_Y));
  mrb_define_const(mrb, c, "Z", mrb_fixnum_value(ALLEGRO_KEY_Z));
  mrb_define_const(mrb, c, "NUM_0", mrb_fixnum_value(ALLEGRO_KEY_0));
  mrb_define_const(mrb, c, "NUM_1", mrb_fixnum_value(ALLEGRO_KEY_1));
  mrb_define_const(mrb, c, "NUM_2", mrb_fixnum_value(ALLEGRO_KEY_2));
  mrb_define_const(mrb, c, "NUM_3", mrb_fixnum_value(ALLEGRO_KEY_3));
  mrb_define_const(mrb, c, "NUM_4", mrb_fixnum_value(ALLEGRO_KEY_4));
  mrb_define_const(mrb, c, "NUM_5", mrb_fixnum_value(ALLEGRO_KEY_5));
  mrb_define_const(mrb, c, "NUM_6", mrb_fixnum_value(ALLEGRO_KEY_6));
  mrb_define_const(mrb, c, "NUM_7", mrb_fixnum_value(ALLEGRO_KEY_7));
  mrb_define_const(mrb, c, "NUM_8", mrb_fixnum_value(ALLEGRO_KEY_8));
  mrb_define_const(mrb, c, "NUM_9", mrb_fixnum_value(ALLEGRO_KEY_9));

  mrb_define_const(mrb, c, "PAD_0", mrb_fixnum_value(ALLEGRO_KEY_PAD_0));
  mrb_define_const(mrb, c, "PAD_1", mrb_fixnum_value(ALLEGRO_KEY_PAD_1));
  mrb_define_const(mrb, c, "PAD_2", mrb_fixnum_value(ALLEGRO_KEY_PAD_2));
  mrb_define_const(mrb, c, "PAD_3", mrb_fixnum_value(ALLEGRO_KEY_PAD_3));
  mrb_define_const(mrb, c, "PAD_4", mrb_fixnum_value(ALLEGRO_KEY_PAD_4));
  mrb_define_const(mrb, c, "PAD_5", mrb_fixnum_value(ALLEGRO_KEY_PAD_5));
  mrb_define_const(mrb, c, "PAD_6", mrb_fixnum_value(ALLEGRO_KEY_PAD_6));
  mrb_define_const(mrb, c, "PAD_7", mrb_fixnum_value(ALLEGRO_KEY_PAD_7));
  mrb_define_const(mrb, c, "PAD_8", mrb_fixnum_value(ALLEGRO_KEY_PAD_8));
  mrb_define_const(mrb, c, "PAD_9", mrb_fixnum_value(ALLEGRO_KEY_PAD_9));

  mrb_define_const(mrb, c, "F1", mrb_fixnum_value(ALLEGRO_KEY_F1));
  mrb_define_const(mrb, c, "F2", mrb_fixnum_value(ALLEGRO_KEY_F2));
  mrb_define_const(mrb, c, "F3", mrb_fixnum_value(ALLEGRO_KEY_F3));
  mrb_define_const(mrb, c, "F4", mrb_fixnum_value(ALLEGRO_KEY_F4));
  mrb_define_const(mrb, c, "F5", mrb_fixnum_value(ALLEGRO_KEY_F5));
  mrb_define_const(mrb, c, "F6", mrb_fixnum_value(ALLEGRO_KEY_F6));
  mrb_define_const(mrb, c, "F7", mrb_fixnum_value(ALLEGRO_KEY_F7));
  mrb_define_const(mrb, c, "F8", mrb_fixnum_value(ALLEGRO_KEY_F8));
  mrb_define_const(mrb, c, "F9", mrb_fixnum_value(ALLEGRO_KEY_F9));
  mrb_define_const(mrb, c, "F10", mrb_fixnum_value(ALLEGRO_KEY_F10));
  mrb_define_const(mrb, c, "F11", mrb_fixnum_value(ALLEGRO_KEY_F11));
  mrb_define_const(mrb, c, "F12", mrb_fixnum_value(ALLEGRO_KEY_F12));

  mrb_define_const(mrb, c, "ESCAPE", mrb_fixnum_value(ALLEGRO_KEY_ESCAPE));
  mrb_define_const(mrb, c, "TILDE", mrb_fixnum_value(ALLEGRO_KEY_TILDE));
  mrb_define_const(mrb, c, "MINUS", mrb_fixnum_value(ALLEGRO_KEY_MINUS));
  mrb_define_const(mrb, c, "EQUALS", mrb_fixnum_value(ALLEGRO_KEY_EQUALS));
  mrb_define_const(mrb, c, "BACKSPACE", mrb_fixnum_value(ALLEGRO_KEY_BACKSPACE));
  mrb_define_const(mrb, c, "TAB", mrb_fixnum_value(ALLEGRO_KEY_TAB));
  mrb_define_const(mrb, c, "OPENBRACE", mrb_fixnum_value(ALLEGRO_KEY_OPENBRACE));
  mrb_define_const(mrb, c, "CLOSEBRACE", mrb_fixnum_value(ALLEGRO_KEY_CLOSEBRACE));
  mrb_define_const(mrb, c, "ENTER", mrb_fixnum_value(ALLEGRO_KEY_ENTER));
  mrb_define_const(mrb, c, "SEMICOLON", mrb_fixnum_value(ALLEGRO_KEY_SEMICOLON));
  mrb_define_const(mrb, c, "QUOTE", mrb_fixnum_value(ALLEGRO_KEY_QUOTE));
  mrb_define_const(mrb, c, "BACKSLASH", mrb_fixnum_value(ALLEGRO_KEY_BACKSLASH));
  mrb_define_const(mrb, c, "BACKSLASH2", mrb_fixnum_value(ALLEGRO_KEY_BACKSLASH2));
  mrb_define_const(mrb, c, "COMMA", mrb_fixnum_value(ALLEGRO_KEY_COMMA));
  mrb_define_const(mrb, c, "FULLSTOP", mrb_fixnum_value(ALLEGRO_KEY_FULLSTOP));
  mrb_define_const(mrb, c, "SLASH", mrb_fixnum_value(ALLEGRO_KEY_SLASH));
  mrb_define_const(mrb, c, "SPACE", mrb_fixnum_value(ALLEGRO_KEY_SPACE));
  mrb_define_const(mrb, c, "INSERT", mrb_fixnum_value(ALLEGRO_KEY_INSERT));
  mrb_define_const(mrb, c, "DELETE", mrb_fixnum_value(ALLEGRO_KEY_DELETE));
  mrb_define_const(mrb, c, "HOME", mrb_fixnum_value(ALLEGRO_KEY_HOME));
  mrb_define_const(mrb, c, "END", mrb_fixnum_value(ALLEGRO_KEY_END));
  mrb_define_const(mrb, c, "PGUP", mrb_fixnum_value(ALLEGRO_KEY_PGUP));
  mrb_define_const(mrb, c, "PGDN", mrb_fixnum_value(ALLEGRO_KEY_PGDN));
  mrb_define_const(mrb, c, "LEFT", mrb_fixnum_value(ALLEGRO_KEY_LEFT));
  mrb_define_const(mrb, c, "RIGHT", mrb_fixnum_value(ALLEGRO_KEY_RIGHT));
  mrb_define_const(mrb, c, "UP", mrb_fixnum_value(ALLEGRO_KEY_UP));
  mrb_define_const(mrb, c, "DOWN", mrb_fixnum_value(ALLEGRO_KEY_DOWN));

  mrb_define_const(mrb, c, "PAD_SLASH", mrb_fixnum_value(ALLEGRO_KEY_PAD_SLASH));
  mrb_define_const(mrb, c, "PAD_ASTERISK", mrb_fixnum_value(ALLEGRO_KEY_PAD_ASTERISK));
  mrb_define_const(mrb, c, "PAD_MINUS", mrb_fixnum_value(ALLEGRO_KEY_PAD_MINUS));
  mrb_define_const(mrb, c, "PAD_PLUS", mrb_fixnum_value(ALLEGRO_KEY_PAD_PLUS));
  mrb_define_const(mrb, c, "PAD_DELETE", mrb_fixnum_value(ALLEGRO_KEY_PAD_DELETE));
  mrb_define_const(mrb, c, "PAD_ENTER", mrb_fixnum_value(ALLEGRO_KEY_PAD_ENTER));

  mrb_define_const(mrb, c, "PRINTSCREEN", mrb_fixnum_value(ALLEGRO_KEY_PRINTSCREEN));
  mrb_define_const(mrb, c, "PAUSE", mrb_fixnum_value(ALLEGRO_KEY_PAUSE));

  mrb_define_const(mrb, c, "ABNT_C1", mrb_fixnum_value(ALLEGRO_KEY_ABNT_C1));
  mrb_define_const(mrb, c, "YEN", mrb_fixnum_value(ALLEGRO_KEY_YEN));
  mrb_define_const(mrb, c, "KANA", mrb_fixnum_value(ALLEGRO_KEY_KANA));
  mrb_define_const(mrb, c, "CONVERT", mrb_fixnum_value(ALLEGRO_KEY_CONVERT));
  mrb_define_const(mrb, c, "NOCONVERT", mrb_fixnum_value(ALLEGRO_KEY_NOCONVERT));
  mrb_define_const(mrb, c, "AT", mrb_fixnum_value(ALLEGRO_KEY_AT));
  mrb_define_const(mrb, c, "CIRCUMFLEX", mrb_fixnum_value(ALLEGRO_KEY_CIRCUMFLEX));
  mrb_define_const(mrb, c, "COLON2", mrb_fixnum_value(ALLEGRO_KEY_COLON2));
  mrb_define_const(mrb, c, "KANJI", mrb_fixnum_value(ALLEGRO_KEY_KANJI));

  mrb_define_const(mrb, c, "PAD_EQUALS", mrb_fixnum_value(ALLEGRO_KEY_PAD_EQUALS));
  mrb_define_const(mrb, c, "BACKQUOTE", mrb_fixnum_value(ALLEGRO_KEY_BACKQUOTE));
  mrb_define_const(mrb, c, "SEMICOLON2", mrb_fixnum_value(ALLEGRO_KEY_SEMICOLON2));
  mrb_define_const(mrb, c, "COMMAND", mrb_fixnum_value(ALLEGRO_KEY_COMMAND));
  mrb_define_const(mrb, c, "UNKNOWN", mrb_fixnum_value(ALLEGRO_KEY_UNKNOWN));

  mrb_define_const(mrb, c, "MODIFIERS", mrb_fixnum_value(ALLEGRO_KEY_MODIFIERS));

  mrb_define_const(mrb, c, "LSHIFT", mrb_fixnum_value(ALLEGRO_KEY_LSHIFT));
  mrb_define_const(mrb, c, "RSHIFT", mrb_fixnum_value(ALLEGRO_KEY_RSHIFT));
  mrb_define_const(mrb, c, "LCTRL", mrb_fixnum_value(ALLEGRO_KEY_LCTRL));
  mrb_define_const(mrb, c, "RCTRL", mrb_fixnum_value(ALLEGRO_KEY_RCTRL));
  mrb_define_const(mrb, c, "ALT", mrb_fixnum_value(ALLEGRO_KEY_ALT));
  mrb_define_const(mrb, c, "ALTGR", mrb_fixnum_value(ALLEGRO_KEY_ALTGR));
  mrb_define_const(mrb, c, "LWIN", mrb_fixnum_value(ALLEGRO_KEY_LWIN));
  mrb_define_const(mrb, c, "RWIN", mrb_fixnum_value(ALLEGRO_KEY_RWIN));
  mrb_define_const(mrb, c, "MENU", mrb_fixnum_value(ALLEGRO_KEY_MENU));
  mrb_define_const(mrb, c, "SCROLLLOCK", mrb_fixnum_value(ALLEGRO_KEY_SCROLLLOCK));
  mrb_define_const(mrb, c, "NUMLOCK", mrb_fixnum_value(ALLEGRO_KEY_NUMLOCK));
  mrb_define_const(mrb, c, "CAPSLOCK", mrb_fixnum_value(ALLEGRO_KEY_CAPSLOCK));

}
