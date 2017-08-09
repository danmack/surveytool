/*
** tabulate.cc
**
** for each file in the results directory
**   o  read it into a result object
**   o  for each query in theSurvey
**     o  record/count the answer in our 'report' structure
*/

#include <stdio.h>
#include <dirent.h>

#include "survey.h"
#include "result.h"
#include "report.h"

void
tabulate(SURVEY &theSurvey, char *resultdir)
{
  DIR           *dirp;
  struct dirent *direntp;
  FILE          *fpResult;
  char          fname[256];
  RESULT        theResult(theSurvey.getname());

  dirp = opendir(resultdir);

  /*
  ** skip over the dot and dot-dot directories
  */

  readdir(dirp);
  readdir(dirp);

  /*
  ** iterate on each file in the directory
  */

  while ((direntp = readdir(dirp)) != NULL)
  {
    sprintf(fname, "%s/%s", resultdir, direntp->d_name);
    fpResult = fopen(fname, "r");
    theResult.readFromDisk(fpResult);
    fclose(fpResult);

    cout << "Just read " << fname << "\t Survey is ";
    if (theResult.getStatus() == COMPLETED)
      cout << "Completed.\n";
    else
      cout << "Not Completed.\n";
  }
}
