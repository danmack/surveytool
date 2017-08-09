// question.h
// a representation of the text of the question

#ifndef _QUESTION_
#define _QUESTION_

#include <iostream>
#include <stdio.h>
#include "getput.h"

typedef class QUESTION *P_QUESTION, **PP_QUESTION;
class QUESTION {
public:
  QUESTION();
  QUESTION(char *qtext);
  void settext(char *newtext);
  char * gettext();
  void print();
  void operator=(QUESTION q1);
  void pversion();
  void readFromDisk(FILE *fp);
  void writeToDisk(FILE *fp);
private:
  char question_text[1024];
  int  version;
};

#endif

