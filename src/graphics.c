#include "minigame.h"

#include <allegro5/allegro_primitives.h>

#include "mruby/array.h"
#include "mruby/hash.h"

static ALLEGRO_COLOR default_color;

static mrb_value
graphics_line(mrb_state *mrb, mrb_value self)
{
  mrb_float x1, y1, x2, y2;
  mrb_value color;
  mrb_float line_size = 1.0;

  ALLEGRO_COLOR draw_color = default_color;

  mrb_get_args(mrb, "ffffo|f", &x1, &y1, &x2, &y2, &color, &line_size);

  if (mrb_array_p(color)) {
    MINIGAME_EXPAND_COLOR_ARRAY(color, draw_color)
  }
  else {
    mrb_data_check_type(mrb, color, &g_minigame_color_t);
    draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
  }

  al_draw_line(x1, y1, x2, y2, draw_color, line_size);

  return mrb_nil_value();
}

static mrb_value
graphics_rect(mrb_state *mrb, mrb_value self)
{
  mrb_float x, y, w, h;
  mrb_value color;
  mrb_float line_size;

  ALLEGRO_COLOR draw_color = default_color;

  mrb_get_args(mrb, "ffffof", &x, &y, &w, &h, &color, &line_size);

  if (mrb_array_p(color)) {
    MINIGAME_EXPAND_COLOR_ARRAY(color, draw_color)
  }
  else {
    mrb_data_check_type(mrb, color, &g_minigame_color_t);
    draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
  }

  al_draw_rectangle(x, y, x+w, y+h, draw_color, line_size);

  return mrb_nil_value();
}

static mrb_value
graphics_rectfill(mrb_state *mrb, mrb_value self)
{
  mrb_float x, y, w, h;
  mrb_value color;

  ALLEGRO_COLOR draw_color = default_color;

  mrb_get_args(mrb, "ffffo", &x, &y, &w, &h, &color);

  if (mrb_array_p(color)) {
    MINIGAME_EXPAND_COLOR_ARRAY(color, draw_color)
  }
  else {
    mrb_data_check_type(mrb, color, &g_minigame_color_t);
    draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
  }

  al_draw_filled_rectangle(x, y, x+w, y+h, draw_color);

  return mrb_nil_value();
}

static mrb_value
graphics_circle(mrb_state *mrb, mrb_value self)
{
  mrb_float x, y, r;
  mrb_value color;
  mrb_float line_size;

  ALLEGRO_COLOR draw_color = default_color;

  mrb_get_args(mrb, "fffo|f", &x, &y, &r, &color, &line_size);

  if (mrb_array_p(color)) {
    MINIGAME_EXPAND_COLOR_ARRAY(color, draw_color)
  }
  else {
    mrb_data_check_type(mrb, color, &g_minigame_color_t);
    draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
  }

  //al_draw_filled_circle(x, y, r, draw_color);
  al_draw_circle(x, y, r, draw_color, line_size);

  return mrb_nil_value();
}

static mrb_value
graphics_circlefill(mrb_state *mrb, mrb_value self)
{
  mrb_float x, y, r;
  mrb_value color;

  ALLEGRO_COLOR draw_color = default_color;

  mrb_get_args(mrb, "fffo", &x, &y, &r, &color);

  if (mrb_array_p(color)) {
    MINIGAME_EXPAND_COLOR_ARRAY(color, draw_color)
  }
  else {
    mrb_data_check_type(mrb, color, &g_minigame_color_t);
    draw_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
  }

  al_draw_filled_circle(x, y, r, draw_color);

  return mrb_nil_value();
}

void
minigame_graphics_init(mrb_state *mrb, struct RClass *parent)
{
  struct RClass *c;

  al_init_primitives_addon();

  c = mrb_define_module_under(mrb, parent, "Graphics");

  mrb_define_module_function(mrb, c, "line", graphics_line, MRB_ARGS_REQ(6));
  mrb_define_module_function(mrb, c, "rect", graphics_rect, MRB_ARGS_REQ(6));
  mrb_define_module_function(mrb, c, "rectfill", graphics_rectfill, MRB_ARGS_REQ(5));
  mrb_define_module_function(mrb, c, "circle", graphics_circle, MRB_ARGS_REQ(5));
  mrb_define_module_function(mrb, c, "circlefill", graphics_circlefill, MRB_ARGS_REQ(4));

  default_color = al_map_rgb(255, 255, 255);
}
