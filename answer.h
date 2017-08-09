// answer.h
// abstract representation of an answer

#ifndef _ANSWER_
#define _ANSWER_

#include "getput.h"
#include <iostream>
#include <stdio.h>

#define _ANSWER_VERSION_  1
#define NUM_ATYPES        7
#define MAX_CHOICES       10
#define MAX_EXCLUDES      16

enum ATYPE
{
  SHORT_TEXT, TEXT,
  CHOOSE,
  INTEGER, FLOAT,
  RANGE, RANK, DUMMY
};

static const char *atype_string[] =
{
  "short-text", "text",
  "choose",
  "integer", "float",
  "range", "rank", "dummy",

  "SHORT-TEXT", "TEXT",
  "CHOOSE",
  "INTEGER", "FLOAT",
  "RANGE", "RANK", "DUMMY"
};

typedef struct tp_choice *P_tp_choice;
struct tp_choice
{
  char *text;
  int num_excludes;
  char *excludes[MAX_EXCLUDES];
};

typedef class ANSWER *P_ANSWER, **PP_ANSWER;
class ANSWER {
public:
  ANSWER();
  ~ANSWER();
  
  void set_version();

// functions used by the scanner

  void settype(char *newtype);
  void settype(ATYPE newtype);
  void setminch(int minch);
  void setmaxch(int maxch);
  void addexclude(char *txt);
  void addchoice(char *txt);

// functions used by the survey tool
  
  int         getnumch();
  int         getminch();
  int         getmaxch();
  P_tp_choice getchoices();
  const char *get_type();
  ATYPE       get_atype();
  void        print();
  void        panswer();
  void        pversion();

  void        readFromDisk(FILE *fp);
  void        writeToDisk(FILE *fp);

// the data

private:
  char         *BigStrTmp;
  int          version;
  ATYPE        answer_type;
  int          num_choices;
  int          min_pick;
  int          max_pick;
  tp_choice    choices[MAX_CHOICES];
};

#endif

