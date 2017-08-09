/*
** initrpt.cc
**
** make an array of RPT_ATOM by iterating through the
** survey passed.  Return a pointer to the array.
**
*/

PP_RPT_ATOM
init_report(SURVEY &theSurvey)
{
  P_RPT_ATOM     atom_array[1024];
  P_RPT_ATOM     atom;

  P_SECTION      thisSection;
  P_QUERY        thisQuery;

  int            section_status;
  int            query_status;
  int            atom_number        = 0;

  section_status = theSurvey->set_to_first_section();
  while (section_status != -1)
  {
    thisSection  = theSurvey->get_current_section();

    query_status = thisSection->set_to_first_query();
    while (query_status != -1)
    {
      thisAnswer = thisSection->get_current_answer();
      queryType  = thisAnswer->get_atype();

      atom_array[atom_number++] = new RPT_ATOM(queryType);
    }

  }
  return atom_array;
}
