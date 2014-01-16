#include "minigame.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "mruby/array.h"
#include "mruby/hash.h"

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
font_draw(mrb_state *mrb, mrb_value self)
{
  mrb_float x, y;
  char *text;
  mrb_value align;
  mrb_value opt;
  mrb_value color;
  int align_flags = ALLEGRO_ALIGN_LEFT;
  int argc;

  ALLEGRO_FONT *font;
  ALLEGRO_COLOR draw_color = default_color;

  argc = mrb_get_args(mrb, "ffz|H", &x, &y, &text, &opt);

  font = (ALLEGRO_FONT*)DATA_PTR(self);

  if (argc > 3) {
    int i;
    mrb_value keys = mrb_funcall(mrb, opt, "keys", 0);

    for (i = 0; i < RARRAY_LEN(keys); ++i) {
      mrb_sym sym;
      mrb_value k = RARRAY_PTR(keys)[i];

      if (mrb_string_p(k))
        sym = mrb_intern_str(mrb, k);
      else
        sym = mrb_symbol(k);

      if (sym == sym_align) {
        align = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(align)) {
          mrb_sym a;
          if (mrb_string_p(align))
            a = mrb_intern_str(mrb, align);
          else
            a = mrb_symbol(align);

          if (a == sym_align_left)
            align_flags = ALLEGRO_ALIGN_LEFT;
          else if (a == sym_align_center)
            align_flags = ALLEGRO_ALIGN_CENTRE;
          else if (a == sym_align_right)
            align_flags = ALLEGRO_ALIGN_RIGHT;
          else
            mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", align);
        }
      }
      else if (sym == sym_color) {
        color = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(color)) {
          mrb_check_datatype(mrb, color, &g_minigame_color_t);
          draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
        }
      }
      else
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", k);
    }
  }

  al_draw_text(font, draw_color, x, y, align_flags, text);

  return mrb_nil_value();
}

static mrb_value
font_text_width(mrb_state *mrb, mrb_value self)
{
  char *text;

  ALLEGRO_FONT *font;

  mrb_get_args(mrb, "z", &text);

  font = (ALLEGRO_FONT*)DATA_PTR(self);

  return mrb_fixnum_value(al_get_text_width(font, text));
}

static mrb_value
font_h(mrb_state *mrb, mrb_value self)
{
  ALLEGRO_FONT *font = (ALLEGRO_FONT*)DATA_PTR(self);

  return mrb_fixnum_value(al_get_font_line_height(font));
}

void
minigame_font_init(mrb_state *mrb, struct RClass *parent)
{
  al_init_font_addon();
  al_init_ttf_addon();

  default_color = al_map_rgb(255, 255, 255);

  font_cls = mrb_define_class_under(mrb, parent, "Font", mrb->object_class);
  MRB_SET_INSTANCE_TT(font_cls, MRB_TT_DATA);

  mrb_define_class_method(mrb, font_cls, "load", font_load, MRB_ARGS_REQ(2));

  mrb_define_method(mrb, font_cls, "draw", font_draw, MRB_ARGS_ARG(3, 1));
  mrb_define_method(mrb, font_cls, "text_width", font_text_width, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, font_cls, "h", font_h, MRB_ARGS_NONE());

  sym_align = mrb_intern_cstr(mrb, "align");
  sym_align_left = mrb_intern_cstr(mrb, "left");
  sym_align_center = mrb_intern_cstr(mrb, "center");
  sym_align_right = mrb_intern_cstr(mrb, "right");
  sym_color = mrb_intern_cstr(mrb, "color");
}
