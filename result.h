// result.h
// the results (or answers) from a particular survey taker

#ifndef _RESULT_
#define _RESULT_

#include <stdio.h>
#include <string.h>
#include "response.h"

enum R_STATUS { NOT_COMPLETED, COMPLETED };

typedef class RESULT *P_RESULT, **PP_RESULT;
class RESULT {
public:
  RESULT(char *s,int max);
  ~RESULT();

  void         addResponse(P_RESPONSE r, int n);
  int          setTo1stResponse();
  void         setExcluded(int n, char **s);

  void         setBookmark(int sn, int qn, int n) { section_number = sn;
						    query_number = qn;
						    raw_location = n;
						  };
  void         markCompleted() { status = COMPLETED; };
  void         markNotCompleted() { status = NOT_COMPLETED; };
  
  P_RESPONSE   getNextResponse();
  int          getNumResponses();
  int          getLastQueryNum();
  int          getLastSectionNum();
  int          getNumExcluded() { return num_excluded; };
  char       **getExcludeList() { return thoseExcluded; };
  R_STATUS     getStatus()      { return status; };

  void         readFromDisk(FILE *fp);
  void         writeToDisk(FILE *fp);

private:
  char         survey_name[128];
  char        *thoseExcluded[128];
  int          num_excluded;
  int          num_responses;
  R_STATUS     status;
  int          section_number;
  int          query_number;
  int          raw_location;
  int          cur_response;
  P_RESPONSE   theResponses[128];
};

#endif

