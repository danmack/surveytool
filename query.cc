// query.cc

#include "query.h"
#include <string.h>

using namespace std;

char * strncpy();

void QUERY::set_version()
{
  version = 1;
}

QUERY::QUERY()
{
  set_version();
  query_name[0] = '\0';
}

QUERY::QUERY(char *name)
{
  set_version();
  strncpy(query_name, name, 127);
  query_name[127] = '\0';
}
  
void QUERY::setname(char * newname)
{
  strncpy(query_name, newname, 127);
  query_name[127] = '\0';
}

void QUERY::setquestion(P_QUESTION q)
{
  query_question = *q;
}

void QUERY::setanswer(P_ANSWER a)
{
  query_answer = *a;
}

char * QUERY::getname()
{
  return strdup(query_name);
}

P_QUESTION QUERY::getquestion()
{
  return &query_question;
}

P_ANSWER QUERY::getanswer()
{
  return &query_answer;
}

void QUERY::print()
{
  cout << "QUERY NAME " << query_name <<
    "  QUERY VERSION #" << version << "\n";
  query_question.pversion();
  query_answer.pversion();
}

void QUERY::readFromDisk(FILE *fp)
{
  fscanf(fp, "%d", &version);
  fscanf(fp, "\n");
  fgetstr(fp, query_name);
  fscanf(fp, "\n");
  query_question.readFromDisk(fp);
  query_answer.readFromDisk(fp);
}

void QUERY::writeToDisk(FILE *fp)
{
  fprintf(fp, "%d\n", version);
  fputstr(fp, query_name);
  fprintf(fp, "\n");
  query_question.writeToDisk(fp);
  query_answer.writeToDisk(fp);
}




