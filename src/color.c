#include "minigame.h"

#include <allegro5/allegro_color.h>

struct RClass *g_minigame_color_cls = NULL;

mrb_data_type g_minigame_color_t = {"Color", mrb_free};

#define RGB_CLAMP(x) \
if (x < 0) x = 0; \
else if (x > 255) x = 255;

static mrb_value
color_rgb(mrb_state *mrb, mrb_value self)
{
  mrb_int r, g, b, a = 255;

  ALLEGRO_COLOR *color;

  mrb_get_args(mrb, "iii|i", &r, &g, &b, &a);

  RGB_CLAMP(r);
  RGB_CLAMP(g);
  RGB_CLAMP(b);
  RGB_CLAMP(a);

  color = (ALLEGRO_COLOR*)mrb_malloc(mrb, sizeof(ALLEGRO_COLOR));
  *color = al_map_rgba(r, g, b, a);

  return mrb_obj_value(mrb_data_object_alloc(mrb, g_minigame_color_cls, color, &g_minigame_color_t));
}

static mrb_value
color_set_r(mrb_state *mrb, mrb_value self)
{
  unsigned char r, g, b, a;
  mrb_int c;
  ALLEGRO_COLOR *color;

  mrb_get_args(mrb, "i", &c);

  RGB_CLAMP(c);

  color = (ALLEGRO_COLOR*)DATA_PTR(self);

  al_unmap_rgba(*color, &r, &g, &b, &a);

  *color = al_map_rgba(c, g, b, a);

  return mrb_fixnum_value(c);
}

static mrb_value
color_set_g(mrb_state *mrb, mrb_value self)
{
  unsigned char r, g, b, a;
  mrb_int c;
  ALLEGRO_COLOR *color;

  mrb_get_args(mrb, "i", &c);

  RGB_CLAMP(c);

  color = (ALLEGRO_COLOR*)DATA_PTR(self);

  al_unmap_rgba(*color, &r, &g, &b, &a);

  *color = al_map_rgba(r, c, b, a);

  return mrb_fixnum_value(c);
}

static mrb_value
color_set_b(mrb_state *mrb, mrb_value self)
{
  unsigned char r, g, b, a;
  mrb_int c;
  ALLEGRO_COLOR *color;

  mrb_get_args(mrb, "i", &c);

  RGB_CLAMP(c);

  color = (ALLEGRO_COLOR*)DATA_PTR(self);

  al_unmap_rgba(*color, &r, &g, &b, &a);

  *color = al_map_rgba(r, g, c, a);

  return mrb_fixnum_value(c);
}

static mrb_value
color_set_a(mrb_state *mrb, mrb_value self)
{
  unsigned char r, g, b, a;
  mrb_int c;
  ALLEGRO_COLOR *color;

  mrb_get_args(mrb, "i", &c);

  RGB_CLAMP(c);

  color = (ALLEGRO_COLOR*)DATA_PTR(self);

  al_unmap_rgba(*color, &r, &g, &b, &a);

  *color = al_map_rgba(r, g, b, c);

  return mrb_fixnum_value(c);
}

#define MINIGAME_COLOR_GET(str) \
static mrb_value \
color_get_ ## str (mrb_state *mrb, mrb_value self) \
{ \
  ALLEGRO_COLOR *color; \
  \
  color = (ALLEGRO_COLOR*)DATA_PTR(self); \
  \
  return mrb_fixnum_value(color->str * 255.0f); \
}

MINIGAME_COLOR_GET(r)

MINIGAME_COLOR_GET(g)

MINIGAME_COLOR_GET(b)

MINIGAME_COLOR_GET(a)

#define HSV_H_CLAMP(x) \
if (x <= 0) x = 0; \
else if (x > 360) x = 360;

#define HSV_SV_CLAMP(x) \
if (x < 0) x = 0; \
else if (x > 1) x = 1;

static mrb_value
color_hsv(mrb_state *mrb, mrb_value self)
{
  mrb_float h = 360.0, s = 1.0, v = 1.0;

  ALLEGRO_COLOR *color;
  
  mrb_get_args(mrb, "|fff", &h, &s, &v);

  HSV_H_CLAMP(h);
  HSV_SV_CLAMP(s);
  HSV_SV_CLAMP(v);

  color = (ALLEGRO_COLOR*)mrb_malloc(mrb, sizeof(ALLEGRO_COLOR));
  *color = al_color_hsv(h, s, v);

  return mrb_obj_value(mrb_data_object_alloc(mrb, g_minigame_color_cls, color, &g_minigame_color_t));
}

static mrb_value
color_set_h(mrb_state *mrb, mrb_value self)
{
  float h, s, v;
  mrb_float c;

  ALLEGRO_COLOR *color;

  mrb_get_args(mrb, "f", &c);

  HSV_H_CLAMP(c);

  color = (ALLEGRO_COLOR*)DATA_PTR(self);

  al_color_rgb_to_hsv(color->r, color->g, color->b, &h, &s, &v);

  al_color_hsv_to_rgb(c, s, v, &color->r, &color->g, &color->b);

  al_color_rgb_to_hsv(color->r, color->g, color->b, &h, &s, &v);
  return mrb_float_value(mrb, h);
}

static mrb_value
color_set_s(mrb_state *mrb, mrb_value self)
{
  float h, s, v;
  mrb_float c;

  ALLEGRO_COLOR *color;

  mrb_get_args(mrb, "f", &c);

  HSV_SV_CLAMP(c);

  color = (ALLEGRO_COLOR*)DATA_PTR(self);

  al_color_rgb_to_hsv(color->r, color->g, color->b, &h, &s, &v);

  al_color_hsv_to_rgb(h, c, v, &color->r, &color->g, &color->b);

  al_color_rgb_to_hsv(color->r, color->g, color->b, &h, &s, &v);
  return mrb_float_value(mrb, s);
}

static mrb_value
color_set_v(mrb_state *mrb, mrb_value self)
{
  float h, s, v;
  mrb_float c;

  ALLEGRO_COLOR *color;

  mrb_get_args(mrb, "f", &c);

  HSV_SV_CLAMP(c);

  color = (ALLEGRO_COLOR*)DATA_PTR(self);

  al_color_rgb_to_hsv(color->r, color->g, color->b, &h, &s, &v);

  al_color_hsv_to_rgb(h, s, c, &color->r, &color->g, &color->b);

  al_color_rgb_to_hsv(color->r, color->g, color->b, &h, &s, &v);
  return mrb_float_value(mrb, v);
}

#define MINIGAME_HSV_GET(str) \
static mrb_value \
color_get_ ## str (mrb_state *mrb, mrb_value self) \
{ \
  float h, s, v; \
  \
  ALLEGRO_COLOR *color = (ALLEGRO_COLOR*)DATA_PTR(self); \
  \
  al_color_rgb_to_hsv(color->r, color->g, color->b, &h, &s, &v); \
  \
  return mrb_float_value(mrb, str); \
}

MINIGAME_HSV_GET(h);
MINIGAME_HSV_GET(s);
MINIGAME_HSV_GET(v);

void
minigame_color_init(mrb_state *mrb, struct RClass *parent)
{
  g_minigame_color_cls = mrb_define_class_under(mrb, parent, "Color", mrb->object_class);
  MRB_SET_INSTANCE_TT(g_minigame_color_cls, MRB_TT_DATA);

  mrb_define_class_method(mrb, g_minigame_color_cls, "rgb", color_rgb, MRB_ARGS_ARG(3, 1));

  mrb_define_method(mrb, g_minigame_color_cls, "r=", color_set_r, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, g_minigame_color_cls, "g=", color_set_g, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, g_minigame_color_cls, "b=", color_set_b, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, g_minigame_color_cls, "a=", color_set_a, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, g_minigame_color_cls, "r", color_get_r, MRB_ARGS_NONE());
  mrb_define_method(mrb, g_minigame_color_cls, "g", color_get_g, MRB_ARGS_NONE());
  mrb_define_method(mrb, g_minigame_color_cls, "b", color_get_b, MRB_ARGS_NONE());
  mrb_define_method(mrb, g_minigame_color_cls, "a", color_get_a, MRB_ARGS_NONE());

  mrb_define_class_method(mrb, g_minigame_color_cls, "hsv", color_hsv, MRB_ARGS_OPT(3));
  mrb_define_method(mrb, g_minigame_color_cls, "h=", color_set_h, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, g_minigame_color_cls, "s=", color_set_s, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, g_minigame_color_cls, "v=", color_set_v, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, g_minigame_color_cls, "h", color_get_h, MRB_ARGS_NONE());
  mrb_define_method(mrb, g_minigame_color_cls, "s", color_get_s, MRB_ARGS_NONE());
  mrb_define_method(mrb, g_minigame_color_cls, "v", color_get_v, MRB_ARGS_NONE());
}
