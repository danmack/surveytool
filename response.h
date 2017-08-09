// response.h
// this class holds the data for a giving response to a particular
// question

#ifndef _RESPONSE_
#define _RESPONSE_

#include <stdio.h>
#include <string.h>
#include "answer.h"

enum YN { YES, NONE };

typedef union RES_DATA *P_RES_DATA, **PP_RES_DATA;
union RES_DATA {
  char  *text;
  int   numInt;
  float numFloat;
  int   choices[10];
};

typedef class RESPONSE *P_RESPONSE, **PP_RESPONSE;
class RESPONSE {
private:
  ATYPE    response_type;
  RES_DATA response_data;
  YN       comment_status;
  char     *comment;
public:
  RESPONSE();
  RESPONSE(ATYPE rtype);
  ~RESPONSE();
  void  setData(char *s);
  void  setData(int i);
  void  setData(float f);
  void  setData(int i, int *iArray);

  void  setComment(char *s);

  ATYPE getType();
  void  setType(ATYPE rtype) { response_type = rtype; };
  const char *getTypeName();

  int   getIntData();
  float getFloatData();
  char  *getTextData();
  int   *getChoiceRankData();
  char  *getComment();

  void  readFromDisk(FILE *fp);
  void  writeToDisk(FILE *fp);
};

#endif

