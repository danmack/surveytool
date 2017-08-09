// section.h
// a section holds a bunch of queries

#ifndef _SECTION_
#define _SECTION_

#include <iostream>
#include <string>

#include "query.h"
#include "question.h"
#include "answer.h"
#include "getput.h"
#include <stdio.h>

#define _SECTION_VERSION_  1
#define MAX_QUERIES        128

typedef class SECTION *P_SECTION, **PP_SECTION;
class SECTION {
public:
  SECTION();
  ~SECTION();
  void       setname(char *newname);
  void       add_query(P_QUERY nquery);
  void       set_version();
  void       gotoQuery(int n);
  int        set_to_first_query();
  int        set_to_next_query();

  char       *getname();
  char       *get_current_query_name();
  P_QUESTION get_current_question();
  P_ANSWER   get_current_answer();
  P_QUERY    get_current_query() { return ar_queries[cur_query]; };
  int getNumQueries() { return num_queries; };
  int getNumCurQuery() { return cur_query + 1; };

  void readFromDisk(FILE *fp);
  void writeToDisk(FILE *fp);

private:
  int          version;
  char         section_name[128];
  int          num_queries;
  int          cur_query;
  P_QUERY      ar_queries[MAX_QUERIES];
};

#endif

