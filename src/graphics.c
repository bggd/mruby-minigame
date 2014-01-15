#include "minigame.h"

#include <allegro5/allegro_primitives.h>

#include "mruby/array.h"
#include "mruby/hash.h"

static ALLEGRO_COLOR default_color;

static mrb_sym sym_fill;
static mrb_sym sym_color;
static mrb_sym sym_size;

static mrb_value
graphics_line(mrb_state *mrb, mrb_value self)
{
  mrb_int x1, y1, x2, y2;
  mrb_value color;
  mrb_value size;
  mrb_value opt;
  int argc;
  mrb_float line_size = 1.0;

  ALLEGRO_COLOR draw_color = default_color;

  ALLEGRO_VERTEX v[2] = {0};

  argc = mrb_get_args(mrb, "iiii|H", &x1, &y1, &x2, &y2, &opt);

  if (argc > 4) {
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

      if (sym == sym_color) {
        color = mrb_hash_get(mrb, opt, k);
        mrb_data_check_type(mrb, color, &g_minigame_color_t);
        draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
      }
      else if (sym == sym_size) {
        size = mrb_hash_get(mrb, opt, k);
        line_size = mrb_float(mrb_Float(mrb, size));
      }
      else
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", k);
    }
  }

  al_draw_line(x1+0.5f, y1+0.5f, x2+0.5f, y2+0.5f, draw_color, line_size);

  v[0].x = x1 + 0.5f;
  v[0].y = y1 + 0.5f;
  v[0].color = draw_color;
  v[1].x = x2 + 0.5f;
  v[1].y = y2 + 0.5f;
  v[1].color = draw_color;

  al_draw_prim(v, NULL, 0, 0, 2, ALLEGRO_PRIM_POINT_LIST);

  return mrb_nil_value();
}

static mrb_value
graphics_rect(mrb_state *mrb, mrb_value self)
{
  mrb_int x, y, w, h;
  mrb_value fill;
  mrb_value color;
  mrb_value size;
  mrb_value opt;
  int argc;
  mrb_float line_size = 1.0;

  bool fill_mode = false;
  ALLEGRO_COLOR draw_color = default_color;

  argc = mrb_get_args(mrb, "iiii|H", &x, &y, &w, &h, &opt);

  if (argc > 4) {
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

      if (sym == sym_fill) {
        fill = mrb_hash_get(mrb, opt, k);
        if (mrb_bool(fill)) fill_mode = true;
      }
      else if (sym == sym_color) {
        color = mrb_hash_get(mrb, opt, k);
        mrb_data_check_type(mrb, color, &g_minigame_color_t);
        draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
      }
      else if (sym == sym_size) {
        size = mrb_hash_get(mrb, opt, k);
        line_size = mrb_float(mrb_Float(mrb, size));
      }
      else
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", k);
    }
  }

  if (fill_mode)
    al_draw_filled_rectangle(x, y, x+w, y+h, draw_color);
  else
    al_draw_rectangle(x+0.5f, y+0.5f, x+w-0.5f, y+h-0.5f, draw_color, line_size);

  return mrb_nil_value();
}

static mrb_value
graphics_circle(mrb_state *mrb, mrb_value self)
{
  mrb_float x, y, r;
  mrb_value fill;
  mrb_value color;
  mrb_value size;
  mrb_value opt;
  int argc;
  mrb_float line_size = 1.0;

  bool fill_mode = false;
  ALLEGRO_COLOR draw_color = default_color;

  argc = mrb_get_args(mrb, "fff|H", &x, &y, &r, &opt);

  if (argc > 3) {
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

      if (sym == sym_fill) {
        fill = mrb_hash_get(mrb, opt, k);
        if (mrb_bool(fill)) fill_mode = true;
      }
      else if (sym == sym_color) {
        color = mrb_hash_get(mrb, opt, k);
        mrb_data_check_type(mrb, color, &g_minigame_color_t);
        draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
      }
      else if (sym == sym_size) {
        size = mrb_hash_get(mrb, opt, k);
        line_size = mrb_float(mrb_Float(mrb, size));
      }
      else
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", k);
    }
  }

  if (fill_mode)
    al_draw_filled_circle(x, y, r, draw_color);
  else
    al_draw_circle(x, y, r, draw_color, line_size);

  return mrb_nil_value();
}

void
minigame_graphics_init(mrb_state *mrb, struct RClass *parent)
{
  struct RClass *c;

  al_init_primitives_addon();

  c = mrb_define_module_under(mrb, parent, "Graphics");

  mrb_define_module_function(mrb, c, "line", graphics_line, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, c, "rect", graphics_rect, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, c, "circle", graphics_circle, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(1));

  default_color = al_map_rgb(255, 255, 255);

  sym_fill = mrb_intern_cstr(mrb, "fill");
  sym_color = mrb_intern_cstr(mrb, "color");
  sym_size = mrb_intern_cstr(mrb, "size");
}
