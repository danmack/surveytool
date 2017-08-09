// question.cc

#include "question.h"
#include <string.h>
using namespace std;

QUESTION::QUESTION()
{
  question_text[0] = '\0';
  version = 1;
}

QUESTION::QUESTION(char *qtext)
{
  strncpy(question_text, qtext, 1023);
  question_text[1023] = '\0';
  version = 1;
}

void QUESTION::settext(char *newtext)
{
  strncpy(question_text, newtext, 1023);
  question_text[1023] = '\0';
}

char * QUESTION::gettext()
{
  return strdup(question_text);
}

void QUESTION::print()
{
  cout << "Question, version " << version << "\n[" << question_text << "]\n";
}

void QUESTION::pversion()
{
  cout << "QUESTION CLASS version " << version << "\n";
}

// overloaded operators

void QUESTION::operator=(QUESTION q1)
{
  strncpy(question_text, q1.gettext(), 1023);
  question_text[1023] = '\0';
}

void QUESTION::readFromDisk(FILE *fp)
{
  fscanf(fp, "%d", &version);
  fscanf(fp, "\n");
  fgetstr(fp, question_text);
  fscanf(fp, "\n");
}

void QUESTION::writeToDisk(FILE *fp)
{
  fprintf(fp, "%d\n", version);
  fputstr(fp, question_text);
  fprintf(fp, "\n");
}
