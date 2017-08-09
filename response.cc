// responce.cc

#include "response.h"

extern "C" void exit(int);

RESPONSE::RESPONSE() {
  response_type = INTEGER;
  comment_status = NONE;
  comment = (char *) 0;
}

RESPONSE::RESPONSE(ATYPE rtype) {
  response_type = rtype;
  comment_status = NONE;
  comment = (char *) 0;
}


RESPONSE::~RESPONSE() {
  if (comment)
    delete comment;

  if (response_type == TEXT || response_type == SHORT_TEXT)
    if (response_data.text)
      delete response_data.text;
}


void
RESPONSE::setComment(char *s) {
  comment_status = YES;
  comment = s;
}

void
RESPONSE::setData(char *s) {
  response_data.text = s;
}

void RESPONSE::setData(int i) {
  response_data.numInt = i;
}

void RESPONSE::setData(float f) {
  response_data.numFloat = f;
}

void RESPONSE::setData(int i, int *iArray) {
  int j;

  for (j = 0; j < 10; j++)
  {
    response_data.choices[j] = iArray[j];
  }
}

ATYPE RESPONSE::getType() {
  return response_type;
}

const char * RESPONSE::getTypeName() {
  if (response_type == DUMMY)
    return "DUMMY";
  else
    return atype_string[response_type];
}

float RESPONSE::getFloatData() {
  return response_data.numFloat;
}

char * RESPONSE::getTextData() {
  if (response_data.text)
    return strdup(response_data.text);
  else
    return (char *) 0;
}

char * RESPONSE::getComment()
{
  return comment;
}

int *RESPONSE::getChoiceRankData() {
  return response_data.choices;
}

void RESPONSE::readFromDisk(FILE *fp) {
  int i;
  char tmp, tmp2;

  fscanf(fp, "%d %d\n", &response_type, &comment_status);

  switch (response_type)
  {
  case INTEGER:
    fscanf(fp, "%d\n", &response_data.numInt);
    break;
  case FLOAT:
    fscanf(fp, "%d\n", &response_data.numFloat);
    break;
  case TEXT:
  case SHORT_TEXT:
    response_data.text = new char[4096];
    fgetstr(fp, response_data.text);
    fscanf(fp, "%c", &tmp);
    break;
  case RANK:
  case CHOOSE:
    for (i = 0; i < 10; i++)
      fscanf(fp, "%d", &response_data.choices[i]);
    fscanf(fp, "%c", &tmp);
    fscanf(fp, "%c", &tmp);
  case DUMMY:
    break;
  default:
    fprintf(stderr, "unknown response-type while reading, dying...\n");
    exit(1);
  }

  if (comment_status == YES)
  {
    comment = new char[2048];
    fgetstr(fp, comment);
    fscanf(fp, "%c", &tmp);
  }

}

void RESPONSE::writeToDisk(FILE *fp) {
  int i;

  fprintf(fp, "%d %d\n", response_type, comment_status);

  switch (response_type)
  {
  case INTEGER:
    fprintf(fp, "%d\n", response_data.numInt);
    break;
  case FLOAT:
    fprintf(fp, "%d\n", response_data.numFloat);
    break;
  case TEXT:
  case SHORT_TEXT:
    fputstr(fp, response_data.text);
    fprintf(fp, "\n");
    break;
  case RANK:
  case CHOOSE:
    for (i = 0; i < 10; i++)
      fprintf(fp, "%d ", response_data.choices[i]);
    fprintf(fp, "\n");
    break;
  case DUMMY:
    break;
  default:
    fprintf(stderr, "unknown response-type while writing, dying...\n");
    exit(1);
  }

  if (comment_status == YES)
  {
    fputstr(fp, comment);
    fprintf(fp, "\n");
  }
}


