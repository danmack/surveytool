/*
** Count the number of queries in a survey
*/

#include "survey.h"

int
countQueries(P_SURVEY thesurvey,
	     P_QUERY all_queries[10][50],
	     int *snum,
	     int *qpSection)
{
  int section_result, query_result;
  P_SECTION thisSection;
  int counter = 0;

  int qnum = 0;
  *snum = 0;
  
  section_result = thesurvey->set_to_first_section();
  thisSection    = thesurvey->get_current_section();
  while (section_result != -1)
  {
    qnum = 0;
    query_result   = thisSection->set_to_first_query();
    while (query_result != -1)
    {
      all_queries[*snum][qnum++] = thisSection->get_current_query();
      query_result = thisSection->set_to_next_query();
      counter++;
    }
    qpSection[(*snum)++] = qnum;
    section_result = thesurvey->set_to_next_section();
    if (section_result != -1) {
      thisSection    = thesurvey->get_current_section();
      query_result   = thisSection->set_to_first_query();
    }
  }
  return counter;
}
