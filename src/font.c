#include "minigame.h"

#include <allegro5/allegro_ttf.h>

#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/variable.h>

static struct RClass *font_cls = NULL;

static ALLEGRO_COLOR default_color;

static mrb_sym sym_align;
static mrb_sym sym_align_left;
static mrb_sym sym_align_center;
static mrb_sym sym_align_right;
static mrb_sym sym_color;

static void
font_free(mrb_state *mrb, void *p)
{
  if (p) {
    ALLEGRO_FONT *font = (ALLEGRO_FONT*)p;
    al_destroy_font(font);
  }
}

static mrb_data_type font_t = {"Font", font_free};

static mrb_value
font_load(mrb_state *mrb, mrb_value self)
{
  char *filepath;
  mrb_int size;

  ALLEGRO_FONT *font;

  mrb_get_args(mrb, "zi", &filepath, &size);

  font = al_load_font(filepath, size, 0);

  if (font)
    return mrb_obj_value(mrb_data_object_alloc(mrb, font_cls, font, &font_t));

  return mrb_nil_value();
}

static mrb_value
font_builtin(mrb_state *mrb, mrb_value self)
{
  return mrb_mod_cv_get(mrb, font_cls, mrb_intern_lit(mrb, "builtin"));
}

static mrb_value
font_draw(mrb_state *mrb, mrb_value self)
{
  mrb_float x, y;
  char *text;
  mrb_value color;
  mrb_value align;
  mrb_value opt;
  ALLEGRO_COLOR tint = default_color;
  int align_flag = ALLEGRO_ALIGN_LEFT;
  mrb_int argc;

  ALLEGRO_FONT *font;

  argc = mrb_get_args(mrb, "ffz|H", &x, &y, &text, &opt);

  if (argc > 3) {
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

      if (sym == sym_color) {
        color = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(color)) {
          if (mrb_array_p(color)) {
            MINIGAME_EXPAND_COLOR_ARRAY(color, tint);
          }
          else {
            mrb_data_check_type(mrb, color, &g_minigame_color_t);
            tint = *((ALLEGRO_COLOR*)DATA_PTR(color));
          }
        }
      }
      else if (sym == sym_align) {
        align = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(align)) {
          mrb_sym a;
          if (mrb_string_p(align))
            a = mrb_intern_str(mrb, align);
          else
            a = mrb_symbol(align);

          if (a == sym_align_left)
            align_flag = ALLEGRO_ALIGN_LEFT;
          else if (a == sym_align_center)
            align_flag = ALLEGRO_ALIGN_CENTRE;
          else if (a == sym_align_right)
            align_flag = ALLEGRO_ALIGN_RIGHT;
        }
        else
          mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %s", k);
      }
    }
  }

  font = (ALLEGRO_FONT*)DATA_PTR(self);

  al_draw_text(font, tint, x, y, align_flag, text);

  return mrb_nil_value();
}

void
minigame_font_init(mrb_state *mrb, struct RClass *parent)
{
  ALLEGRO_FONT *builtin;

  al_init_font_addon();
  al_init_ttf_addon();

  default_color = al_map_rgb(255, 255, 255);

  font_cls = mrb_define_class_under(mrb, parent, "Font", mrb->object_class);
  MRB_SET_INSTANCE_TT(font_cls, MRB_TT_DATA);


  mrb_define_class_method(mrb, font_cls, "load", font_load, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, font_cls, "builtin", font_builtin, MRB_ARGS_NONE());

  mrb_undef_method(mrb, font_cls, "initialize");

  mrb_define_method(mrb, font_cls, "puts", font_draw, MRB_ARGS_ARG(3, 1));

  sym_align = mrb_intern_lit(mrb, "align");
  sym_align_left = mrb_intern_lit(mrb, "left");
  sym_align_center = mrb_intern_lit(mrb, "center");
  sym_align_right = mrb_intern_lit(mrb, "right");
  sym_color = mrb_intern_lit(mrb, "color");

  builtin = al_create_builtin_font();
  mrb_value v = mrb_obj_value(mrb_data_object_alloc(mrb, font_cls, builtin, &font_t));
  mrb_mod_cv_set(mrb, font_cls, mrb_intern_lit(mrb, "builtin"), v);
}

