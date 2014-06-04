#include "minigame.h"

#include <allegro5/allegro_image.h>

#include "mruby/hash.h"
#include "mruby/array.h"
#include "mruby/value.h"

static struct RClass *image_cls = NULL;

static ALLEGRO_COLOR default_color;

static mrb_sym sym_angle;
static mrb_sym sym_color;
static mrb_sym sym_region;
static mrb_sym sym_scale;
static mrb_sym sym_anchor;
static mrb_sym sym_pivot;
static mrb_sym sym_flip;

static void
image_free(mrb_state *mrb, void *p)
{
  if (p) {
    ALLEGRO_BITMAP *bitmap = (ALLEGRO_BITMAP*)p;

    al_destroy_bitmap(bitmap);
  }
}

static mrb_data_type image_t = {"Image", image_free};

static mrb_value
image_new(mrb_state *mrb, mrb_value self)
{
  mrb_int width, height;
  mrb_value color;
  int argc;

  ALLEGRO_BITMAP *bitmap;
  ALLEGRO_COLOR new_color = default_color;

  argc = mrb_get_args(mrb, "ii|o", &width, &height, &color);

  if (argc > 2) {
    if (mrb_array_p(color)) {
      MINIGAME_EXPAND_COLOR_ARRAY(color, new_color)
    }
    else {
      mrb_data_check_type(mrb, color, &g_minigame_color_t);
      new_color = *((ALLEGRO_COLOR*)DATA_PTR(color));
    }
  }

  bitmap = al_create_bitmap(width, height);

  if (bitmap) {
    al_set_target_bitmap(bitmap);
    al_clear_to_color(new_color);
    al_set_target_backbuffer(al_get_current_display());

    return mrb_obj_value(mrb_data_object_alloc(mrb, image_cls, bitmap, &image_t));
  }

  return mrb_nil_value();
}

static mrb_value
image_load(mrb_state *mrb, mrb_value self)
{
  char *filepath;
  ALLEGRO_BITMAP *bitmap;

  mrb_get_args(mrb, "z", &filepath);

  bitmap = al_load_bitmap(filepath);
  
  if (bitmap)
    return mrb_obj_value(mrb_data_object_alloc(mrb, image_cls, bitmap, &image_t));

  return mrb_nil_value();
}

static mrb_value
image_target(mrb_state *mrb, mrb_value self)
{
  mrb_value key;
  mrb_value blk;
  ALLEGRO_BITMAP *bitmap;

  mrb_get_args(mrb, "o&", &key, &blk);

  if (mrb_nil_p(blk)) return mrb_nil_value();

  mrb_data_check_type(mrb, key, &image_t);
  bitmap = (ALLEGRO_BITMAP*)DATA_PTR(key);

  al_set_target_bitmap(bitmap);
  mrb_yield_argv(mrb, blk, 0, NULL);
  al_set_target_backbuffer(al_get_current_display());

  return mrb_nil_value();
}

static mrb_value
image_hold_drawing(mrb_state *mrb, mrb_value self)
{
  mrb_bool b;

  mrb_get_args(mrb, "b", &b);

  if (b) al_hold_bitmap_drawing(true);
  else al_hold_bitmap_drawing(false);

  return mrb_bool_value(b);
}

static mrb_value
image_is_hold_drawing(mrb_state *mrb, mrb_value self)
{
  if (al_is_bitmap_drawing_held()) return mrb_true_value();
  else return mrb_false_value();
}

static mrb_value
image_w(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(al_get_bitmap_width((ALLEGRO_BITMAP*)DATA_PTR(self)));
}

static mrb_value
image_h(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(al_get_bitmap_height((ALLEGRO_BITMAP*)DATA_PTR(self)));
}

static mrb_value
image_clear(mrb_state *mrb, mrb_value self)
{
  mrb_value color;
  int argc;

  ALLEGRO_BITMAP *bitmap;
  ALLEGRO_COLOR clear_color = default_color;

  argc = mrb_get_args(mrb, "|o", &color);

  bitmap = (ALLEGRO_BITMAP*)DATA_PTR(self);

  if (argc > 0) {
    if (mrb_array_p(color)) {
      MINIGAME_EXPAND_COLOR_ARRAY(color, clear_color)
    }
    else {
      mrb_data_check_type(mrb, color, &g_minigame_color_t);
      clear_color = *((ALLEGRO_COLOR*)DATA_PTR(self));
    }
  }

  al_set_target_bitmap(bitmap);
  al_clear_to_color(clear_color);
  al_set_target_backbuffer(al_get_current_display());

  return mrb_nil_value();
}

static mrb_value
image_draw(mrb_state *mrb, mrb_value self)
{
  mrb_float x, y;
  mrb_value angle;
  mrb_value color;
  mrb_value region;
  mrb_value scale;
  mrb_value anchor;
  mrb_value pivot;
  mrb_value flip;
  mrb_value opt;

  ALLEGRO_BITMAP *bitmap;
  mrb_float angle_rad = 0.0;
  ALLEGRO_COLOR tint = default_color;
  mrb_int src_x = 0, src_y = 0, src_w = -1, src_h = -1;
  mrb_float scale_x = 1.0, scale_y = 1.0;
  mrb_float anchor_x = 0.0, anchor_y = 0.0;
  mrb_float pivot_x = 0.5, pivot_y = 0.5;
  int flip_xy = 0;

  mrb_float w, h;
  mrb_float ws, hs;
  mrb_float cx, cy;
  mrb_float dx, dy;

  int argc;
 
  argc = mrb_get_args(mrb, "ff|H", &x, &y, &opt);

  bitmap = (ALLEGRO_BITMAP*)DATA_PTR(self);

  if (bitmap == NULL) {
    return mrb_nil_value();
  }

  if (argc > 2) { 
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

      if (sym == sym_angle) {
        angle = mrb_hash_get(mrb, opt, k);
        angle_rad = mrb_fixnum(mrb_Integer(mrb, angle)) * (ALLEGRO_PI/180.0);
      }
      else if (sym == sym_color) {
        color = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(color)) {
          if (mrb_array_p(color)) {
            MINIGAME_EXPAND_COLOR_ARRAY(color, tint)
          }
          else {
            mrb_data_check_type(mrb, color, &g_minigame_color_t);
            tint = *((ALLEGRO_COLOR*)DATA_PTR(color));
          }
        }
      }
      else if (sym == sym_region) {
        region = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(region)) {
          mrb_check_type(mrb, region, MRB_TT_ARRAY);
          src_x = mrb_fixnum(mrb_Integer(mrb, RARRAY_PTR(region)[0]));
          src_y = mrb_fixnum(mrb_Integer(mrb, RARRAY_PTR(region)[1]));
          src_w = mrb_fixnum(mrb_Integer(mrb, RARRAY_PTR(region)[2]));
          src_h = mrb_fixnum(mrb_Integer(mrb, RARRAY_PTR(region)[3]));
          if (src_w < 0) src_w = 0;
          if (src_h < 0) src_h = 0;
        }
      }
      else if (sym == sym_scale) {
        scale = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(scale)) {
          if (mrb_array_p(scale)) {
            scale_x = mrb_float(mrb_Float(mrb, RARRAY_PTR(scale)[0]));
            scale_y = mrb_float(mrb_Float(mrb, RARRAY_PTR(scale)[1]));
          }
          else
            scale_x = scale_y = mrb_float(mrb_Float(mrb, scale));
        }
      }
      else if (sym == sym_anchor) {
        anchor = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(anchor)) {
          if (mrb_array_p(anchor)) {
            anchor_x = mrb_float(mrb_Float(mrb, RARRAY_PTR(anchor)[0]));
            anchor_y = mrb_float(mrb_Float(mrb, RARRAY_PTR(anchor)[1]));
          }
          else
            anchor_x = anchor_y = mrb_float(mrb_Float(mrb, anchor));
        }
      }
      else if (sym == sym_pivot) {
        pivot = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(pivot)) {
          if (mrb_array_p(pivot)) {
            pivot_x = mrb_float(mrb_Float(mrb, RARRAY_PTR(pivot)[0]));
            pivot_y = mrb_float(mrb_Float(mrb, RARRAY_PTR(pivot)[1]));
          }
          else
            pivot_x = pivot_y = mrb_float(mrb_Float(mrb, pivot));
        }
      }
      else if (sym == sym_flip) {
        flip = mrb_hash_get(mrb, opt, k);
        if (!mrb_nil_p(flip)) {
          mrb_check_type(mrb, flip, MRB_TT_ARRAY);
          flip_xy |= mrb_bool(RARRAY_PTR(flip)[0]) ? ALLEGRO_FLIP_HORIZONTAL : 0;
          flip_xy |= mrb_bool(RARRAY_PTR(flip)[1]) ? ALLEGRO_FLIP_VERTICAL : 0;
        }
      }
      else
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown keyword: %S", k);
    }
  }

  w = (src_w > -1) ? src_w : al_get_bitmap_width(bitmap);
  h = (src_h > -1) ? src_h : al_get_bitmap_height(bitmap);

  cx = w * pivot_x;
  cy = h * pivot_y;
  
  ws = w * scale_x;
  hs = h * scale_y;

  dx = x + (ws * pivot_x) - (ws * anchor_x);
  dy = y + (hs * pivot_y) - (hs * anchor_y);

  al_draw_tinted_scaled_rotated_bitmap_region(bitmap,src_x, src_y, w, h, tint, cx, cy, dx, dy, scale_x, scale_y, angle_rad, flip_xy);

  return mrb_nil_value();
}

static mrb_value
image_sub_image(mrb_state *mrb, mrb_value self)
{
  mrb_int x, y, w, h;

  ALLEGRO_BITMAP *bitmap;
  ALLEGRO_BITMAP *sub_bitmap;

  mrb_get_args(mrb, "iiii", &x, &y, &w, &h);

  bitmap = (ALLEGRO_BITMAP*)DATA_PTR(self);

  sub_bitmap = al_create_sub_bitmap(bitmap, x, y, w, h);

  if (sub_bitmap) 
    return mrb_obj_value(mrb_data_object_alloc(mrb, image_cls, sub_bitmap, &image_t));

  return mrb_nil_value();
}

static mrb_value
image_convert_mask_to_alpha(mrb_state *mrb, mrb_value self)
{
  mrb_value color;

  ALLEGRO_COLOR mask_color;

  ALLEGRO_BITMAP *bitmap;

  mrb_get_args(mrb, "o", &color);

  bitmap = (ALLEGRO_BITMAP*)DATA_PTR(self);

  if (mrb_array_p(color)) {
    MINIGAME_EXPAND_COLOR_ARRAY(color, mask_color);
  }
  else {
    mrb_data_check_type(mrb, color, &g_minigame_color_t);
    mask_color = *(ALLEGRO_COLOR*)DATA_PTR(color);
  }

  al_convert_mask_to_alpha(bitmap, mask_color);

  return mrb_nil_value();
}

void
minigame_image_init(mrb_state *mrb, struct RClass *parent)
{
  al_init_image_addon();

  image_cls = mrb_define_class_under(mrb, parent, "Image", mrb->object_class);
  MRB_SET_INSTANCE_TT(image_cls, MRB_TT_DATA);

  mrb_define_class_method(mrb, image_cls, "new", image_new, MRB_ARGS_ARG(2, 1));
  mrb_define_class_method(mrb, image_cls, "load", image_load, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, image_cls, "target", image_target, MRB_ARGS_REQ(1) | MRB_ARGS_BLOCK());
  mrb_define_class_method(mrb, image_cls, "hold_drawing", image_hold_drawing, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, image_cls, "hold_drawing?", image_is_hold_drawing, MRB_ARGS_NONE());

  mrb_define_method(mrb, image_cls, "w", image_w, MRB_ARGS_NONE());
  mrb_define_method(mrb, image_cls, "h", image_h, MRB_ARGS_NONE());

  mrb_define_method(mrb, image_cls, "clear", image_clear, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, image_cls, "draw", image_draw, MRB_ARGS_ARG(2, 1));
  mrb_define_method(mrb, image_cls, "sub_image", image_sub_image, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, image_cls, "convert_mask_to_alpha", image_convert_mask_to_alpha, MRB_ARGS_REQ(1));

  default_color = al_map_rgb(255, 255, 255);

  sym_angle = mrb_intern_lit(mrb, "angle");
  sym_color = mrb_intern_lit(mrb, "color");
  sym_region = mrb_intern_lit(mrb, "region");
  sym_scale = mrb_intern_lit(mrb, "scale");
  sym_anchor = mrb_intern_lit(mrb, "anchor");
  sym_pivot = mrb_intern_lit(mrb, "pivot");
  sym_flip = mrb_intern_lit(mrb, "flip");
}
