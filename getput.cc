#include "getput.h"
#include <stdlib.h>

void fputstr(FILE *fp, char *s)
{
  fprintf(fp, "%s%c", s, '\0');
}

void fgetstr(FILE *fp, char *s)
{
  int done, l = 0;
  char tmp, tmp2;
  int fres;

  done = 0;
  while (!done)
  {
    fres = fscanf(fp, "%c", &tmp);

    if (fres == 0 || fres == EOF) {
      fprintf(stderr, "ERROR: results data corrupt or otherwise invalid - aborting!\n");
      exit(2);
    }
    
    if (tmp == '\0')
    {
      done = 1;
    }
    else
    {
      s[l] = tmp;
      l++;
      s[l] = '\0';
    }
  }
  s[l] = '\0';
}
