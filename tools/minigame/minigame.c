#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/error.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

extern void error_display(const char *text);

static void
mg_create_console()
{
#ifdef _WIN32
  if (AllocConsole()) {
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "r", stdin);
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
  }
#endif
}

int
main(int argc, char **argv)
{
  mrb_state *mrb;
  mrbc_context *c;
  FILE *fp;

  fp = fopen("init_minigame.rb", "rb");
  if (fp == NULL) {
    error_display("Couldn't load 'init_minigame.rb'");
    return EXIT_FAILURE;
  }

  mrb = mrb_open();
  if (mrb == NULL) {
    error_display("Invalid mrb_state, exiting mruby");
    return EXIT_FAILURE;
  }

  c = mrbc_context_new(mrb);
  mrbc_filename(mrb, c, "init_minigame.rb");

  mrb_load_file_cxt(mrb, fp, c);
  fclose(fp);
  mrbc_context_free(mrb, c);

  if (mrb->exc) {
    mrb_value v;

    v = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);

    mg_create_console();

    mrb_print_error(mrb);

    error_display(RSTRING_PTR(v));
  }

  mrb_close(mrb);

  return EXIT_SUCCESS;
}
