#define ALLEGRO_NO_MAGIC_MAIN
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
  

void
error_display(const char *text)
{
  ALLEGRO_DISPLAY *disp;
  ALLEGRO_EVENT_QUEUE *ev_queue;
  ALLEGRO_FONT *font;

  al_init();
  al_install_keyboard();
  al_install_mouse();
  al_init_font_addon();

  al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
  disp = al_create_display(640, 480);
  al_set_window_title(disp, "mruby-minigame");

  ev_queue = al_create_event_queue();

  al_register_event_source(ev_queue, al_get_display_event_source(disp));
  al_register_event_source(ev_queue, al_get_keyboard_event_source());
  al_register_event_source(ev_queue, al_get_mouse_event_source());

  font = al_create_builtin_font();

  for (;;) {
    ALLEGRO_EVENT ev;
    while (al_get_next_event(ev_queue, &ev)) {
      if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) goto done;
    }

    al_clear_to_color(al_map_rgb(200, 120, 120));
    al_draw_multiline_text(font, al_map_rgb(255, 255, 255), 64, 64, 640-64, 10, ALLEGRO_ALIGN_LEFT, text);
    al_flip_display();
  }
done:
  return;
}
