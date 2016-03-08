#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/error.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <stdlib.h>
#include <stdlib.h>

extern void error_display(const char *text);


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

  v = mrb_load_file_cxt(mrb, fp, c);
  fclose(fp);
  mrbc_context_free(mrb, c);

  if (mrb->exc && !mrb_undef_p(v)) {
    FILE* redirect = freopen("error_info.txt", "w", stderr);
    if (redirect) {
      FILE* error_info;
      char str[512];
      mrb_value text = mrb_str_buf_new(mrb, 0);

      mrb_print_error(mrb);
      fclose(redirect);

      error_info = fopen("error_info.txt", "r");
      while (fgets(str, 512, error_info)) {
        if (str[0] == '\t') str[0] = ' ';
        text = mrb_str_cat_cstr(mrb, text, str);
      }
      fclose(error_info);

      error_display(RSTRING_PTR(text));
    }
  }

  mrb_close(mrb);

  return EXIT_SUCCESS;
}
