/*
** report.h
**
** structures for the report generator
**  the report structure hold all the information to report on
**  one query
*/

#ifndef _REPORT_
#define _REPORT_

#include "answer.h"

struct txt_struct {
  int n;
  char *list[3072];
  char *name[3072];
};

struct ch_struct {
  int    cnt_all[10];
  int  cnt_local[10];
  int cnt_remote[10];
};

typedef union ADATA *P_ADATA, **PP_ADATA;
union ADATA {
  txt_struct txtAnswers;
  ch_struct  chAnswers;
};

typedef struct REPORT *P_REPORT, **PP_REPORT;
struct REPORT {
  ATYPE qtype;
  int num_answered;
  int local_answered;
  int remote_answered;
  int num_comments;
  char *comment_list[3072];
  char *comment_name[3072];
  ADATA theData;
};

#endif

