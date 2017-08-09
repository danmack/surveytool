#include <stdio.h>
#include <dirent.h>

int
main(int argc, char **argv)
{
  DIR *dirp;
  struct dirent *direntp;

  if (argc > 1)
    dirp = opendir(argv[1]);
  else
    dirp = opendir(".");

  while ((direntp = readdir(dirp)) != NULL)
    (void)printf("%s\n", direntp->d_name);
  
  (void)closedir(dirp);
  return(0);
}
