// survey.cc

#include "survey.h"
#include <iostream>

using namespace std;

void SURVEY::set_version()
{
  version = _SURVEY_VERSION_;
  num_sections = 0;
}

SURVEY::SURVEY()
{
  set_version();
  survey_name[0] = '\0';
}

SURVEY::~SURVEY() {
  int i;

  if (intro_text)
    delete intro_text;

  for (i = 0; i < num_sections; i++)
    delete the_sections[i];
}


void SURVEY::setname(char *newname)
{
  strncpy(survey_name, newname, 127);
  survey_name[127] = '\0';
}

int SURVEY::set_to_first_section()
{
  if (num_sections != -1)
    return (cur_section = 0);
  else
    return (cur_section = -1);
}

void SURVEY::gotoSection(int n) {
  cur_section = n - 1;
}

char * SURVEY::get_current_section_name()
{
  if (cur_section != -1)
    return the_sections[cur_section]->getname();
  else
    return (char *) 0;
}

P_SECTION SURVEY::get_current_section()
{
  if (cur_section != -1)
    return the_sections[cur_section];
  else
    return (P_SECTION) 0;
}

int SURVEY::set_to_next_section()
{
  if (cur_section != -1 && (cur_section < (num_sections-1)))
    return(cur_section++);
  else
    return(cur_section = -1);
}

void SURVEY::add_section(P_SECTION new_section)
{
  if (num_sections < MAX_QUERIES)
  {
    the_sections[num_sections] = new_section;
    num_sections++;
  }
  else
  {
    cerr << "Warning: MAX_QUERIES exceeded\n";
  }
}

char * SURVEY::getname()
{
  return strdup(survey_name);
}

char * SURVEY::getIntro()
{
  return intro_text;
}

void SURVEY::psurvey()
{
  cout << "SURVEY CLASS" << "\n";
}

void SURVEY::pversion()
{
  cout << "SURVEY CLASS version " << version << "\n";
}

void SURVEY::readFromDisk(FILE *fp)
{
  int i;
  char tmp;

  fscanf(fp, "%d", &version);
  fscanf(fp, "\n");

  fgetstr(fp, survey_name);
  fscanf(fp, "%c", &tmp);

  intro_text = new char[2048];
  fgetstr(fp, intro_text);
  fscanf(fp, "%d", &num_sections);
  fscanf(fp, "\n");

  for (i = 0; i < num_sections; i++)
  {
    the_sections[i] = new SECTION;
    the_sections[i]->readFromDisk(fp);
  }
}

void SURVEY::writeToDisk(FILE *fp)
{
  int i;
  fprintf(fp, "%d\n", version);
  fputstr(fp, survey_name);
  fprintf(fp, "\n");
  fputstr(fp, intro_text);
  fprintf(fp, "%d\n", num_sections);
  for (i = 0; i < num_sections; i++)
  {
    the_sections[i]->writeToDisk(fp);
  }
}
