#include "minigame.h"

#include "mruby/array.h"
#include "mruby/hash.h"
#include "mruby/string.h"
#include "mruby/variable.h"

static struct RClass *display_cls = NULL;

static ALLEGRO_DISPLAY *disp = NULL;
static ALLEGRO_BITMAP *icon_bitmap = NULL;

static mrb_sym sym_title;
static mrb_sym sym_vsync;
static mrb_sym sym_fullscreen;
static mrb_sym sym_multisample;
static mrb_sym sym_icon;

static mrb_sym current_blender;

static mrb_sym blend_alpha;
static mrb_sym blend_screen;
static mrb_sym blend_additive;
static mrb_sym blend_subtractive;
static mrb_sym blend_premultiplied;
static mrb_sym blend_multiplicative;
static mrb_sym blend_replace;

static mrb_value
display_create(mrb_state *mrb, mrb_value self)
{
  mrb_int width, height;
  const char *title_cstr = NULL;
  const char *icon_filepath = NULL;
  mrb_value icon;
  mrb_value opt;
  int argc;

  argc = mrb_get_args(mrb, "ii|H", &width, &height, &opt);

  al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);

  mrb_mod_cv_set(mrb, display_cls, sym_title, mrb_str_new_lit(mrb, "mruby-minigame"));
  mrb_mod_cv_set(mrb, display_cls, sym_vsync, mrb_false_value());
  mrb_mod_cv_set(mrb, display_cls, sym_fullscreen, mrb_false_value());
  mrb_mod_cv_set(mrb, display_cls, sym_multisample, mrb_fixnum_value(0));
 
  if (argc  > 2) {
    int i;
    mrb_value keys;
    mrb_value v;

    keys = mrb_hash_keys(mrb, opt);

    for (i = 0; i < RARRAY_LEN(keys); ++i) {
      mrb_sym sym;
      mrb_value k = RARRAY_PTR(keys)[i];

      if (mrb_string_p(k))
        sym = mrb_intern_str(mrb, k);
      else
        sym = mrb_symbol(k);

      if (sym == sym_title) {
        v = mrb_hash_get(mrb, opt, k);
        mrb_mod_cv_set(mrb, display_cls, sym_title, v);
      }
      else if (sym == sym_vsync) {
        v = mrb_hash_get(mrb, opt, k);
        mrb_mod_cv_set(mrb, display_cls, sym_vsync, v);
      }
      else if (sym == sym_fullscreen) {
        v = mrb_hash_get(mrb, opt, k);
        mrb_mod_cv_set(mrb, display_cls, sym_fullscreen, v);
      }
      else if (sym == sym_multisample) {
        v = mrb_hash_get(mrb, opt, k);
        mrb_mod_cv_set(mrb, display_cls, sym_multisample, v);
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
 
  if (mrb_bool(mrb_mod_cv_get(mrb, display_cls, sym_vsync)))
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
  else
    al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);

  if (mrb_bool(mrb_mod_cv_get(mrb, display_cls, sym_fullscreen)))
    al_set_new_display_flags(ALLEGRO_FULLSCREEN);
  else
    al_set_new_display_flags(ALLEGRO_WINDOWED);

  if (mrb_fixnum(mrb_mod_cv_get(mrb, display_cls, sym_multisample)) > 0) {
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, mrb_fixnum(mrb_mod_cv_get(mrb, display_cls, sym_multisample)), ALLEGRO_SUGGEST);
  }
  else
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 0, ALLEGRO_SUGGEST);

  if (disp) {
    minigame_unregister_event_source(al_get_display_event_source(disp));
    al_destroy_display(disp);
    disp = NULL;
  }

  disp = al_create_display(width, height);

  if (disp) {
    al_convert_bitmaps();

    al_set_window_title(disp, mrb_str_to_cstr(mrb, mrb_mod_cv_get(mrb, display_cls, sym_title)));

    if (icon_filepath) {
      icon_bitmap = al_load_bitmap(icon_filepath);
      al_set_display_icon(disp, icon_bitmap);
    }

    minigame_register_event_source(al_get_display_event_source(disp));

    /* set default blend mode to :alpha */
    al_set_separate_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

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
display_toggle_fullscreen(mrb_state *mrb, mrb_value self)
{
  mrb_value title = mrb_mod_cv_get(mrb, display_cls, sym_title);
  mrb_value vsync = mrb_mod_cv_get(mrb, display_cls, sym_vsync);
  mrb_value fullscreen = mrb_mod_cv_get(mrb, display_cls, sym_fullscreen);
  mrb_value multisample = mrb_mod_cv_get(mrb, display_cls, sym_multisample);
  mrb_value w = mrb_fixnum_value(al_get_display_width(disp));
  mrb_value h = mrb_fixnum_value(al_get_display_height(disp));
  mrb_value opt = mrb_hash_new_capa(mrb, 4);

  mrb_hash_set(mrb, opt, mrb_symbol_value(sym_title), title);
  mrb_hash_set(mrb, opt, mrb_symbol_value(sym_vsync), vsync);
  mrb_hash_set(mrb, opt, mrb_symbol_value(sym_fullscreen), (mrb_bool(fullscreen)) ? mrb_false_value() : mrb_true_value());
  mrb_hash_set(mrb, opt, mrb_symbol_value(sym_multisample), multisample);

  return mrb_funcall(mrb, mrb_obj_value(display_cls), "create", 3, w, h, opt);
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
  else if (mode == blend_screen)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_SRC_COLOR);
  else if (mode == blend_additive)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ONE);
  else if (mode == blend_subtractive)
    al_set_blender(ALLEGRO_DEST_MINUS_SRC, ALLEGRO_ALPHA, ALLEGRO_ONE);
  else if (mode == blend_premultiplied)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
  else if (mode == blend_multiplicative)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, ALLEGRO_ZERO);
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
display_set_title(mrb_state *mrb, mrb_value self)
{
  mrb_value title;

  mrb_get_args(mrb, "S", &title);

  al_set_window_title(al_get_current_display(), mrb_str_to_cstr(mrb, title));
  mrb_mod_cv_set(mrb, display_cls, sym_title, title);

  return mrb_nil_value();
}

static mrb_value
display_set_icon(mrb_state *mrb, mrb_value self)
{
  mrb_value image;

  mrb_get_args(mrb, "o", &image);

  if (mrb_nil_p(image)) return mrb_nil_value();
  if (!mrb_obj_is_kind_of(mrb, image, mrb_class_get_under(mrb, mrb_module_get(mrb, "Minigame"), "Image")))
    mrb_raisef(mrb, E_ARGUMENT_ERROR, "wrong argument type");

  al_set_display_icon(al_get_current_display(), (ALLEGRO_BITMAP*)DATA_PTR(image));

  return mrb_nil_value();
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

static mrb_value
display_render_target(mrb_state *mrb, mrb_value self)
{
  mrb_value image;
  mrb_value blk;
  ALLEGRO_BITMAP *bitmap;

  mrb_get_args(mrb, "o&", &image, &blk);

  if (mrb_nil_p(blk)) 
    mrb_raisef(mrb, E_ARGUMENT_ERROR, "no block given");

  if (!mrb_obj_is_kind_of(mrb, image, mrb_class_get_under(mrb, mrb_module_get(mrb, "Minigame"), "Image")))
    mrb_raisef(mrb, E_ARGUMENT_ERROR, "wrong argument type");

  bitmap = (ALLEGRO_BITMAP*)DATA_PTR(image);

  al_set_target_bitmap(bitmap);
  mrb_yield_argv(mrb, blk, 0, NULL);
  al_set_target_backbuffer(al_get_current_display());

  return mrb_nil_value();
}

void
minigame_display_init(mrb_state *mrb, struct RClass *parent)
{
  display_cls = mrb_define_module_under(mrb, parent, "Display");

  mrb_define_module_function(mrb, display_cls, "create", display_create, MRB_ARGS_ARG(2, 1));
  mrb_define_module_function(mrb, display_cls, "clear", display_clear, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, display_cls, "flip", display_flip, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, display_cls, "toggle_fullscreen", display_toggle_fullscreen, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, display_cls, "screenshot", display_screenshot, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, display_cls, "set_blender", display_set_blender, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, display_cls, "get_blender", display_get_blender, MRB_ARGS_NONE());
  /* Display.blend_mode is deprecated. */
  mrb_define_alias(mrb, display_cls->c, "blend_mode", "set_blender");
  mrb_define_module_function(mrb, display_cls, "set_title", display_set_title, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, display_cls, "set_icon", display_set_icon, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, display_cls, "w", display_get_w, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, display_cls, "h", display_get_h, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, display_cls, "render_target", display_render_target, MRB_ARGS_REQ(2)|MRB_ARGS_BLOCK());

  blend_alpha = mrb_intern_lit(mrb, "alpha");
  blend_screen = mrb_intern_lit(mrb, "screen");
  blend_additive = mrb_intern_lit(mrb, "additive");
  blend_subtractive = mrb_intern_lit(mrb, "subtractive");
  blend_premultiplied = mrb_intern_lit(mrb, "premultiplied");
  blend_multiplicative = mrb_intern_lit(mrb, "multiplicative");
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
