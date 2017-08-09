// answer.cc

#include "answer.h"
#include <iostream>
#include <string>
using namespace std;

void ANSWER::set_version()
{
  version = _ANSWER_VERSION_;
}

ANSWER::ANSWER()
{
  set_version();
  BigStrTmp = new char[4096];
}

ANSWER::~ANSWER()
{
  int i, j;

  delete BigStrTmp;
  for (i = 0; i < num_choices; i++)
  {
    if (choices[i].text)
      delete choices[i].text;

    for (j = 0; j < choices[i].num_excludes; j++)
      delete choices[i].excludes[j];
  }
}

void ANSWER::print()
{
  int i, j;

  cout << "ANSWER, version " << version <<
    ", type = " << atype_string[answer_type+7] << "\n";

  if ( (answer_type == RANK) || (answer_type == CHOOSE) )
    for (i = 0; i < num_choices; i++)
    {
      cout << "  " << choices[i].text << "\n";
      for (j = 0; j < choices[i].num_excludes; j++)
      {
	cout << "    " << choices[i].excludes[j] << "\n";
      }
    }
  cout << "\n";
}

void ANSWER::settype(ATYPE newtype)
{
  answer_type = newtype;
}

void ANSWER::settype(char *newtype)
{
  int i = 0, j = 0, DONE = 0;

  while (!DONE && i < NUM_ATYPES)
  {
    j = i + NUM_ATYPES;
    if ( (strcmp(newtype, atype_string[i]) == 0) ||
	 (strcmp(newtype, atype_string[j]) == 0))
    {
      answer_type = (ATYPE) i;
      DONE = 1;
    }
    i++;
  }
}

void ANSWER::setminch(int minch)
{
  min_pick = minch;
}

void ANSWER::setmaxch(int maxch)
{
  max_pick = maxch;
}

void ANSWER::addexclude(char *txt)
{
  if (num_choices)
  {
    int n = choices[num_choices-1].num_excludes;
    choices[num_choices-1].num_excludes++;
    choices[num_choices-1].excludes[n] = strdup(txt);

    // strcpy(choices[num_choices-1].excludes[n].text, txt);
  }
}

void ANSWER::addchoice(char *txt)
{
  printf("ANDEBUG [%s]\n", txt);
  choices[num_choices].text = strdup(txt);

  //  strcpy(choices[num_choices].text, txt);
  choices[num_choices].num_excludes = 0;
  num_choices++;
}

int ANSWER::getnumch()
{
  return num_choices;
}

int ANSWER::getminch()
{
  return min_pick;
}

int ANSWER::getmaxch()
{
  return max_pick;
}

P_tp_choice ANSWER::getchoices()
{
  return choices;
}

ATYPE ANSWER::get_atype()
{
  return answer_type;
}

const char * ANSWER::get_type()
{
  return atype_string[answer_type];
}

void ANSWER::panswer()
{
  cout << answer_type << "\n";
}

void ANSWER::pversion()
{
  cout << "ANSWER CLASS version " << version << "\n";
}

void ANSWER::readFromDisk(FILE *fp)
{
  int i,j;

  fscanf(fp, "%d %d %d %d %d", &version, &answer_type, &num_choices,
	 &min_pick, &max_pick);
  fscanf(fp, "\n");

  for ( i = 0; i < num_choices; i++ )
  {
    fgetstr(fp, BigStrTmp);
    choices[i].text = strdup(BigStrTmp);

    fscanf(fp, "%d\n", &choices[i].num_excludes);
    for ( j = 0 ; j < choices[i].num_excludes; j++ )
    {
      fgetstr(fp, BigStrTmp);
      choices[i].excludes[j] = strdup(BigStrTmp);
    }
  }
}

void ANSWER::writeToDisk(FILE *fp)
{
  int i, j;

  fprintf(fp, "%d %d %d %d %d\n", version, answer_type, num_choices,
	  min_pick, max_pick);

  for ( i = 0; i < num_choices; i++ )
  {
    fputstr(fp, choices[i].text);
    fprintf(fp, "%d\n", choices[i].num_excludes);
    for ( j = 0; j < choices[i].num_excludes; j++ )
    {
      fputstr(fp, choices[i].excludes[j]);
    }
  }
}
