#include "minigame.h"

#include "mruby/array.h"
#include "mruby/hash.h"
#include "mruby/string.h"

static ALLEGRO_DISPLAY *disp = NULL;
static ALLEGRO_BITMAP *icon_bitmap = NULL;

static mrb_sym sym_title;
static mrb_sym sym_vsync;
static mrb_sym sym_fullscreen;
static mrb_sym sym_multisample;
static mrb_sym sym_icon;

static mrb_sym current_blender;

static mrb_sym blend_alpha;
static mrb_sym blend_additive;
static mrb_sym blend_subtractive;
static mrb_sym blend_premultiplied;
static mrb_sym blend_replace;

static mrb_value
display_create(mrb_state *mrb, mrb_value self)
{
  mrb_int width, height;
  mrb_value title;
  char *title_cstr = NULL;
  char *icon_filepath = NULL;
  mrb_value vsync;
  mrb_value fullscreen;
  mrb_value multisample;
  mrb_value icon;
  mrb_value opt;
  int argc;

  argc = mrb_get_args(mrb, "ii|H", &width, &height, &opt);

  al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);

  if (argc  > 2) {
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

      if (sym == sym_title) {
        title = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(title)) {
          mrb_check_type(mrb, title, MRB_TT_STRING);
          title_cstr = mrb_string_value_cstr(mrb, &title);
        }
      }
      else if (sym == sym_vsync) {
        vsync = mrb_hash_get(mrb, opt, k);
        if (mrb_bool(vsync)) al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
        else
          al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
      }
      else if (sym == sym_fullscreen) {
        fullscreen = mrb_hash_get(mrb, opt, k);
        if (mrb_bool(fullscreen)) al_set_new_display_flags(ALLEGRO_FULLSCREEN);
      }
      else if (sym == sym_multisample) {
        mrb_int sample;

        multisample = mrb_hash_get(mrb, opt, k);
        sample = mrb_fixnum(mrb_Integer(mrb, multisample));
        al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
        al_set_new_display_option(ALLEGRO_SAMPLES, sample, ALLEGRO_SUGGEST);
      }
      else if (sym == sym_icon) {
        icon = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(icon)) {
          mrb_check_type(mrb, icon, MRB_TT_STRING);
          icon_filepath = mrb_string_value_cstr(mrb, &icon);
        }
      }
      else
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", k);
    }
  }

  disp = al_create_display(width, height);

  if (disp) {
    if (title_cstr) al_set_window_title(disp, title_cstr);
    else al_set_window_title(disp, "mruby-minigame");

    if (icon_filepath) {
      icon_bitmap = al_load_bitmap(icon_filepath);
      al_set_display_icon(disp, icon_bitmap);
    }

    minigame_register_event_source(al_get_display_event_source(disp));

    /* set default blend mode to :alpha */
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    return mrb_true_value();
  }

  return mrb_false_value();
}

static mrb_value
display_clear(mrb_state *mrb, mrb_value self)
{
  mrb_value color_obj;
  int argc;

  argc = mrb_get_args(mrb, "|o", &color_obj);

  if (argc > 0) {
    ALLEGRO_COLOR clear_color;
    if (mrb_array_p(color_obj)) {
      MINIGAME_EXPAND_COLOR_ARRAY(color_obj, clear_color)
    }
    else {
      mrb_data_check_type(mrb, color_obj, &g_minigame_color_t);
      clear_color = *(ALLEGRO_COLOR*)DATA_PTR(color_obj);
    }
    al_clear_to_color(clear_color);
  }
  else
    al_clear_to_color(al_map_rgb(0, 0, 0));

  return mrb_nil_value();
}

static mrb_value
display_flip(mrb_state *mrb, mrb_value self)
{
  al_flip_display();

  return mrb_nil_value();
}

static mrb_value
display_screenshot(mrb_state *mrb, mrb_value self)
{
  char *filepath;

  mrb_get_args(mrb, "z", &filepath);

  if (al_save_bitmap(filepath, al_get_target_bitmap()))
    return mrb_true_value();
  else
    return mrb_false_value();
}

static mrb_value
display_set_blender(mrb_state *mrb, mrb_value self)
{
  mrb_sym mode;

  mrb_get_args(mrb, "n", &mode);

  if (mode == blend_alpha)
    al_set_separate_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
  else if (mode == blend_additive)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ONE);
  else if (mode == blend_subtractive)
    al_set_blender(ALLEGRO_DEST_MINUS_SRC, ALLEGRO_ALPHA, ALLEGRO_ONE);
  else if (mode == blend_premultiplied)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
  else if (mode == blend_replace)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
  else
    mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", mrb_symbol_value(mode));
  
  current_blender = mode;

  return mrb_nil_value();
}

static mrb_value
display_get_blender(mrb_state *mrb, mrb_value self)
{
  return mrb_symbol_value(current_blender);
}

static mrb_value
display_get_w(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(al_get_display_width(disp));
}

static mrb_value
display_get_h(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(al_get_display_height(disp));
}

void
minigame_display_init(mrb_state *mrb, struct RClass *parent)
{
  struct RClass *c;

  c = mrb_define_module_under(mrb, parent, "Display");

  mrb_define_module_function(mrb, c, "create", display_create, MRB_ARGS_ARG(2, 1));
  mrb_define_module_function(mrb, c, "clear", display_clear, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, c, "flip", display_flip, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, c, "screenshot", display_screenshot, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, c, "set_blender", display_set_blender, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, c, "get_blender", display_get_blender, MRB_ARGS_NONE());
  /* Display.blend_mode is deprecated. */
  mrb_define_alias(mrb, c->c, "blend_mode", "set_blender");
  mrb_define_module_function(mrb, c, "w", display_get_w, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, c, "h", display_get_h, MRB_ARGS_NONE());

  blend_alpha = mrb_intern_lit(mrb, "alpha");
  blend_additive = mrb_intern_lit(mrb, "additive");
  blend_subtractive = mrb_intern_lit(mrb, "subtractive");
  blend_premultiplied = mrb_intern_lit(mrb, "premultiplied");
  blend_replace = mrb_intern_lit(mrb, "replace");
  
  current_blender = blend_alpha;

  sym_title = mrb_intern_lit(mrb, "title");
  sym_vsync = mrb_intern_lit(mrb, "vsync");
  sym_fullscreen = mrb_intern_lit(mrb, "fullscreen");
  sym_multisample = mrb_intern_lit(mrb, "multisample");
  sym_icon = mrb_intern_lit(mrb, "icon");
}

void
minigame_display_final(void)
{
  if (disp) {
    al_destroy_display(disp);
  }
}
