// section.cc

#include "section.h"
#include <iostream>
using namespace std;

void SECTION::set_version() {
  version = _SECTION_VERSION_;
  num_queries = 0;
}

SECTION::SECTION() {
  set_version();
  section_name[0] = '\0';
}

SECTION::~SECTION() {
  int i;

  for (i = 0; i < num_queries; i++)
    delete ar_queries[i];
}


void SECTION::setname(char *newname) {
  strncpy(section_name, newname, 127);
  section_name[127] = '\0';
}

void SECTION::gotoQuery(int n) {
  cur_query = n-1;
}

int SECTION::set_to_first_query() {
  if (num_queries != -1)
    return (cur_query = 0);
  else
    return (cur_query = -1);
}

char * SECTION::get_current_query_name() {
  if (cur_query != -1)
    return ar_queries[cur_query]->getname();
  else
    return (char *) 0;
}

P_QUESTION SECTION::get_current_question() {
  if (cur_query != -1)
    return ar_queries[cur_query]->getquestion();
  else
    return (P_QUESTION) 0;
}

P_ANSWER SECTION::get_current_answer() {
  if (cur_query != -1)
    return ar_queries[cur_query]->getanswer();
  else
    return (P_ANSWER) 0;
}

int SECTION::set_to_next_query() {
  if (cur_query != -1 && (cur_query < (num_queries-1)))
    return(cur_query++);
  else
    return(cur_query = -1);
}

void SECTION::add_query(P_QUERY nquery) {
  if (num_queries < MAX_QUERIES)
  {
    ar_queries[num_queries] = nquery;
    num_queries++;
  }
  else
  {
    cerr << "Warning: MAX_QUERIES exceeded\n";
  }
}

char * SECTION::getname() {
  return strdup(section_name);
}

void SECTION::readFromDisk(FILE *fp)
{
  int i;

  fscanf(fp, "%d", &version);
  fscanf(fp, "\n");

  fgetstr(fp, section_name);
  fscanf(fp, "\n");

  fscanf(fp, "%d", &num_queries);
  fscanf(fp, "\n");

  for (i = 0; i < num_queries; i++)
  {
    ar_queries[i] = new QUERY;
    ar_queries[i]->readFromDisk(fp);
  }
}

void SECTION::writeToDisk(FILE *fp)
{
  int i;
  fprintf(fp, "%d\n", version);
  fputstr(fp, section_name);
  fprintf(fp, "\n");
  fprintf(fp, "%d\n", num_queries);
  for (i = 0; i < num_queries; i++)
  {
    ar_queries[i]->writeToDisk(fp);
  }
}
