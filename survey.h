// survey.h
// abstract representation of an survey

#ifndef _SURVEY_
#define _SURVEY_

// EON-HACK: maybe this is not needed anymroe
#include <iostream>
// #include <string.h>

#include "section.h"
#include "query.h"
#include "question.h"
#include "answer.h"
#include "getput.h"
#include <stdio.h>

#define _SURVEY_VERSION_  1

typedef class SURVEY *P_SURVEY, **PP_SURVEY;
class SURVEY {
public:
  SURVEY();
  ~SURVEY();
  void         setname(char *newname);
  void         setIntro(char *newname) { intro_text = newname; };
  void         add_section(P_SECTION new_section);
  void         psurvey();
  void         pversion();
  void         gotoSection(int n);

  void         set_version();
  int          set_to_first_section();
  int          set_to_next_section();
  char *       get_current_section_name();
  P_SECTION    get_current_section();
  char *       getname();
  char *       getIntro();
  int          getNumSections() { return num_sections; };
  int          getNumCurSection() { return cur_section + 1; };

  void         readFromDisk(FILE *fp);
  void         writeToDisk(FILE *fp);

private:
  int          version;
  char         survey_name[128];
  char         *intro_text;
  int          num_sections;
  int          cur_section;
  P_SECTION    the_sections[64];
};

#endif

