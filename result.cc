#include "result.h"

RESULT::RESULT(char *s, int max) {
  strncpy(survey_name, s, 127);
  survey_name[127] = '\0';

  num_responses    = max;
  num_excluded     = 0;
  section_number   = 0;
  query_number     = 0;
  cur_response     = -1;
  thoseExcluded[0] = (char *) 0;
  status = NOT_COMPLETED;
}

RESULT::~RESULT() {
  int i;

  for (i = 0; i < num_excluded; i++)
    delete thoseExcluded[i];

  for (i = 0; i < num_responses; i++)
    delete theResponses[i];
}

void RESULT::addResponse(P_RESPONSE r, int n) {
  theResponses[n] = r;
  num_responses = n+1;
}

void RESULT::setExcluded(int n, char *s[]) {
  int i;

  num_excluded  = n;
  for (i = 0; i < n; i++)
    thoseExcluded[i] = s[i];
}

int RESULT::setTo1stResponse() {
  if (num_responses == 0)
    return (cur_response = -1);
  else
    return (cur_response = 0);
}

P_RESPONSE RESULT::getNextResponse() {
  if (cur_response != -1)
  {
    if (cur_response >= num_responses)
      return (P_RESPONSE) -1;
    else
      return theResponses[cur_response++];
  }
  else
    return (P_RESPONSE) -1;
}

int RESULT::getNumResponses() {
  return raw_location;
}

int RESULT::getLastQueryNum() {
  return query_number;
}

int RESULT::getLastSectionNum() {
  return section_number;
}

void RESULT::readFromDisk(FILE *fp) {
  int i;
  char tmp;

  fgetstr(fp, survey_name);
  fscanf(fp, "%d %d %d %d %d %d\n",
	 &raw_location, &num_responses,
	 &section_number, &query_number,
	 &num_excluded,
	 &status);

  for (i = 0; i < num_excluded; i++)
  {
    thoseExcluded[i] = new char[128];
    fgetstr(fp, thoseExcluded[i]);
  }

  for (i = 0; i < num_responses; i++)
  {
    theResponses[i] = new RESPONSE;
    theResponses[i]->readFromDisk(fp);
  }
}

void RESULT::writeToDisk(FILE *fp) {
  int i;

  fputstr(fp, survey_name);
  fprintf(fp, "%d %d %d %d %d %d\n",
	  raw_location, num_responses,
	  section_number, query_number,
	  num_excluded,
	  status);

  for (i = 0; i < num_excluded; i++)
    fputstr(fp, thoseExcluded[i]);

  fprintf(fp, "\n");

  for (i = 0; i < num_responses; i++)
  {
    theResponses[i]->writeToDisk(fp);
  }
}
