#include <stdlib.h>
#include <stdio.h>

#define Z80_ASM "Z80ASM\032\n"

int main(const int argc, const char *argv[])
{
  FILE *out = stdout;
  FILE *in = stdin;

  fputs(Z80_ASM, out);
  fputc(0, out);
  fputc(0, out);

  int c;
  while ((c = fgetc(in)) != EOF) {
    fputc(c, out);
  }

  fclose(out);
  fclose(in);
  return 0;
}
