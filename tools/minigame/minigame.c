#include <mruby.h>
#include <mruby/compile.h>
#include <stdlib.h>
#include <stdlib.h>


#ifdef _WIN32
#include <windows.h>
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int
main(int argc, char **argv)
#endif
{
  mrb_state *mrb;
  mrbc_context *c;
  mrb_value v;
  FILE *fp;

  fp = fopen("init_minigame.rb", "rb");
  if (fp == NULL) {
    fputs("Couldn't load 'init_minigame.rb'", stderr);
    return EXIT_FAILURE;
  }

  mrb = mrb_open();
  if (mrb == NULL) {
    fputs("Invalid mrb_state, exiting mruby", stderr);
    return EXIT_FAILURE;
  }

  c = mrbc_context_new(mrb);

  v = mrb_load_file_cxt(mrb, fp, c);
  fclose(fp);
  mrbc_context_free(mrb, c);

  if (mrb->exc && !mrb_undef_p(v)) mrb_print_error(mrb);

  return EXIT_SUCCESS;
}
