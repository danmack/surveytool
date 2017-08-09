#include "result.h"

char *f2 = "obj-copy-result";

int
main(int argc, char **argv)
{
  FILE *a = fopen(argv[1], "r");
  FILE *b = fopen(f2, "w");

  RESULT copy("This is a test");

  copy.readFromDisk(a);
  copy.writeToDisk(b);

  fclose(a);
  fclose(b);
}
