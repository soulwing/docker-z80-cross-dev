#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define Z80_ASM "Z80ASM\032\n"

int main(const int argc, const char *argv[])
{
  FILE *in = NULL;
  FILE *out = NULL;

  if (argc > 1) {
    if (*argv[1] == '-') {
      fprintf(stderr, "usage: %s [bin-file [z80-file]]\n", argv[0]);
      exit(1);
    }
    in = fopen(argv[1], "r");
    if (in == NULL) {
      fprintf(stderr, "fopen: %s: %s\n", argv[1], strerror(errno));
      exit(1);
    }
  }
  if (argc > 2) {
    out = fopen(argv[2], "w+");
    if (out == NULL) {
      fprintf(stderr, "fopen: %s: %s\n", argv[2], strerror(errno));
      exit(1);
    }
  }
  if (in == NULL) in = stdin;
  if (out == NULL) out = stdout;

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
