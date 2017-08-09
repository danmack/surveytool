// query.h
// data representation of question/answer pairs

#ifndef _QUERY_
#define _QUERY_

#include <iostream>

#include "question.h"
#include "answer.h"
#include "getput.h"
#include <stdio.h>

typedef class QUERY *P_QUERY, **PP_QUERY;
class QUERY {
public:
  QUERY();
  QUERY(char *name);

  // scanner funcs
  void setname(char * newname);
  void setquestion(P_QUESTION q);
  void setanswer(P_ANSWER a);

  // survey tool funcs
  char * getname();
  P_QUESTION getquestion();
  P_ANSWER getanswer();
  ATYPE getType() { return query_answer.get_atype(); };

  void print();
  void set_version();

  void readFromDisk(FILE *fp);
  void writeToDisk(FILE *fp);

private:
  int          version;
  char         query_name[128];
  QUESTION     query_question;
  ANSWER       query_answer;
  const char * comment;
};

#endif

