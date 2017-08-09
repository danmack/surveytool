/*
** rpgen
** generate a report on results from the survey tool (srv)
*/

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <pwd.h>

#include "survey.h"
#include "result.h"
#include "report.h"

using namespace std;

// extern "C" void exit(int);
// extern "C" int atoi(char *);

extern int countQueries(P_SURVEY, P_QUERY[10][50], int *, int *);

P_QUERY aq[10][50];

int numQueries;
int numSections;
int QperSect[10];

int LOCAL_USER = 0;
int DO_LOCAL_STUFF = 1;
int ANONYMOUS = 0;

P_REPORT all_reports[128];
P_QUERY  all_queries[128];

void
update_report(int s_index, P_RESPONSE the_response, char *user_name)
{
  P_REPORT this_report = all_reports[s_index];
  P_QUERY  this_query  = all_queries[s_index];
  ATYPE the_type = the_response->getType();
  
  int n, i;
  
  int *tmp_choices;
  
  char *comment;
  
  switch (the_type) {
  case DUMMY:
    break;

  case CHOOSE:

    (this_report->num_answered)++;

    if (LOCAL_USER) {
      (this_report->local_answered)++;
    } else {
      (this_report->remote_answered)++;
    }

    comment = the_response->getComment();
    if (comment) {
      if (strlen(comment)) {
        this_report->comment_list[this_report->num_comments] = strdup(comment);
        this_report->comment_name[this_report->num_comments] = strdup(user_name);
        (this_report->num_comments)++;
      } else {
	comment = (char *) 0;
      }
    }

    tmp_choices = the_response->getChoiceRankData();

    for (i = 0; i < 10; i++) {
      if (tmp_choices[i]) {
	(this_report->theData.chAnswers.cnt_all[i])++;
	if (LOCAL_USER) {
	  (this_report->theData.chAnswers.cnt_local[i])++;
        } else {
	  (this_report->theData.chAnswers.cnt_remote[i])++;
        }
      }
    }
    break;

  case TEXT:

    (this_report->num_answered)++;

    if (LOCAL_USER) {
      (this_report->local_answered)++;
    } else {
      (this_report->remote_answered)++;
    }
    
    comment = the_response->getComment();
    if (comment) {
      if (strlen(comment)) {
	this_report->comment_list[this_report->num_comments] = strdup(comment);
	this_report->comment_name[this_report->num_comments] = strdup(user_name);
	(this_report->num_comments)++;
      } else {
	comment = (char *) 0;
      }
    }
      
    n = this_report->theData.txtAnswers.n;
    (this_report->theData.txtAnswers.n)++;
    this_report->theData.txtAnswers.list[n] = the_response->getTextData();
    this_report->theData.txtAnswers.name[n] = strdup(user_name);
    break;
      
  default:
    cerr << "Warning: Unknown query type encountered\n";
  }
}

void
setup_reports()
{
  int i, j, k;
  int c = 0;
  int zz_type;

  /* created a convenience array of the survey queries that is
  ** not a multidimensional array.
  */

  for (i = 0; i < numSections; i++)
    for (j = 0; j < QperSect[i]; j++)
      all_queries[c++] = aq[i][j];
  
  /*
  ** allocate the memory needed for all the reports and set the
  ** type for each one in advance
  */
  
  for (i = 0; i < numQueries; i++)
  {
    all_reports[i] = new REPORT;

    all_reports[i]->qtype = all_queries[i]->getType();
    
    all_reports[i]->num_answered = 0;
    all_reports[i]->local_answered = 0;
    all_reports[i]->remote_answered = 0;

    for (k = 0; k < 3072; k++)
    {
      all_reports[i]->comment_list[k] = (char *) 0;
      all_reports[i]->comment_name[k] = (char *) 0;
    }

    if (all_queries[i]->getType() == CHOOSE)
      for (j = 0; j < 10; j++)
      {
	all_reports[i]->theData.chAnswers.cnt_all[j]    = 0;
	all_reports[i]->theData.chAnswers.cnt_local[j]  = 0;
	all_reports[i]->theData.chAnswers.cnt_remote[j] = 0;
      }
    else
      {
	all_reports[i]->theData.txtAnswers.n = 0;
	for (k = 0; k < 3072; k++)
	{
	  all_reports[i]->theData.txtAnswers.list[k] = (char *) 0;
	  all_reports[i]->theData.txtAnswers.name[k] = (char *) 0;
	}
      }
  }
}

void
run_report(P_SURVEY theSurvey, char *resultdir)
{

/*
** for each file in the results directory
**   o  read it into a result object
**   o  for each query in theSurvey
**     o  record/count the answer in our 'report' structure
*/

  DIR           *dirp;
  struct dirent *direntp;
  struct passwd *pass_entry;
  FILE          *fpResult;
  char          fname[256];
  P_RESULT      theResult;
  P_RESPONSE    the_response;
  char          *uid;
  char          *user_name;
  
  int i;
  
  dirp = opendir(resultdir);
  
  /*
  ** skip over the dot and dot-dot directories
  */

  readdir(dirp);
  readdir(dirp);

  /*
  ** iterate on each file in the directory
  */

  while ((direntp = readdir(dirp)) != NULL)
  {
    theResult = new RESULT("Running a Report", numQueries);

    sprintf(fname, "%s/%s", resultdir, direntp->d_name);

    fpResult = fopen(fname, "r");
    fprintf(stderr, "  reading [%s] ... ", fname);
    if (fpResult) {
      theResult->readFromDisk(fpResult);
    } else {
      fprintf(stderr, "error: got NULL from fopen: [%s]\n", fname);
      exit(1);
    }
    
    fclose(fpResult);

    uid = direntp->d_name;
    /*printf("Reading...%s w/[%s]\n", fname,uid); */


    pass_entry = getpwuid(atoi(uid));
    if (pass_entry) {
      user_name = pass_entry->pw_name;
    } else {
        sprintf(fname, "UID = %s (expired account)", uid);
        user_name = strdup(fname);
    }
    
    /*
    ** check to see if this user is local or not by looking at the answer
    ** to question number 3.
    */

    theResult->setTo1stResponse();

    the_response = theResult->getNextResponse();
    ANONYMOUS = (the_response->getChoiceRankData())[0];

    if (ANONYMOUS)
    {
      delete user_name;
      user_name = strdup("anonymous");
    }

    the_response = theResult->getNextResponse();
    the_response = theResult->getNextResponse();

    if (DO_LOCAL_STUFF)
      LOCAL_USER = (the_response->getChoiceRankData())[0];
    else
      LOCAL_USER = 1;
    
/*
    printf ("LOCAL USER = %d\n", LOCAL_USER);
*/
    theResult->setTo1stResponse();
    for (i = 0; i < theResult->getNumResponses(); i++)
    {
      the_response = theResult->getNextResponse();
      update_report(i, the_response, user_name);
    }
    delete theResult;

  }

  closedir(dirp);
}

void
print_indented(int n, char *s)
{
  int done;
  int end_of_line = 0;
  int i, pos = 0;
  char x;

  int l = strlen(s);
  
  if (strlen(s))
  {
    x = s[strlen(s)-1];
    while (x == ' ' || x == 10 || x == 11 || x == 13)
    {
      s[strlen(s)-1] = '\0';
      x = s[strlen(s)-1];
      l--;
    }
  }
  
  if (strlen(s))
    done = 0;
  else
    done = 1;

  while (!done)
  {
    /*
    **    indent 'n' spaces
    **    print each character until NL
    **   
    */

    for (i = 0; i < n; i++)
      printf(" ");

    end_of_line = 0;
    while (!end_of_line)
    {
      if (s[pos] == 10 || s[pos] == 11 || s[pos] == 13)
	;
      else
	printf("%c", s[pos]);

      if (s[pos] == 10)
      {
	printf("\n");
	end_of_line = 1;
      }

      pos++;

      if (pos >= l)
      {
	end_of_line = 1;
	done = 1;
      }
    }
  }
}

void
displayChoices(int qnum)
{
  char first_line[256];
  char next_line[1024];
  int j, k;

  int numChoices = all_queries[qnum]->getanswer()->getnumch();
  P_tp_choice the_choices= all_queries[qnum]->getanswer()->getchoices();
  int i, pl, pr, pt;
  P_REPORT rpt = all_reports[qnum];

  int local_ans, remote_ans, total_ans;
  int local_cnt, remote_cnt, total_cnt;

  printf("  Choices                                    ");
  printf(" Local  %%  | Remote %%  | Total  %%\n");
  printf("  ========================================== ");
  printf(" ========= | ========= | =========\n");

  for (i = 0; i < numChoices; i++)
  {
    local_ans  = rpt->local_answered;
    remote_ans = rpt->remote_answered;
    total_ans  = rpt->num_answered;
    
    local_cnt  = rpt->theData.chAnswers.cnt_local[i];
    remote_cnt = rpt->theData.chAnswers.cnt_remote[i];
    total_cnt  = rpt->theData.chAnswers.cnt_all[i];

    if (local_ans)
      pl = (int) rint ( ( (double) local_cnt  / (double) local_ans * 100.0 ));
    else
      pl = 0;

    if (remote_ans)
      pr = (int) rint ( ( (double) remote_cnt / (double) remote_ans * 100.0 ));
    else
      pr = 0;

    if (total_ans)
      pt = (int) rint ( ( (double) total_cnt  / (double) total_ans  * 100.0 ));
    else
      pt = 0;

    for (j = 0; j < 80; j++)
      first_line[i] = 0;

    sprintf(first_line, "  %1d.  ", i+1);

    sprintf(next_line, "%s                                      ",
	    the_choices[i].text);

    strncat(first_line, next_line, 38);

    for (j = 5; j < strlen(first_line); j++)
      if (first_line[j] < 32)
      {
	for (k = j; k < strlen(first_line); k++)
	  first_line[k] = ' ';
      }

    printf ("%s  %4d %3d  | %4d %3d  | %4d %3d\n", first_line,
	    local_cnt, pl,
	    remote_cnt, pr,
	    total_cnt, pt);

  }

  printf("\n\n");
  
}

void
displayComments(int qnum)
{
  P_REPORT rpt = all_reports[qnum];
  int numComments = rpt->num_comments;
  int i;
  int real_numComments = 0;
  char *user_name;

  for (i = 0; i < numComments; i++)
     if (strlen(rpt->comment_list[i]) > 2)
       real_numComments++;

  if (real_numComments)
    printf ("COMMENTS: (%d people commented)\n", real_numComments);
  else
    numComments = 0;
  
  for (i = 0; i < numComments; i++)
  {
     if (strlen(rpt->comment_list[i]) > 2)
     {
        user_name = rpt->comment_name[i];
        printf("\n== %s\n", user_name);
        print_indented(2, rpt->comment_list[i]);
        printf("\n");
     }
  }
}

void
displayText(int qnum)
{
  P_REPORT rpt = all_reports[qnum];
  int i;
  int num_txt = rpt->theData.txtAnswers.n;
  int real_num_txt = 0;
  char *user_name;

  for (i = 0; i < num_txt; i++)
     if (strlen(rpt->theData.txtAnswers.list[i]) > 2)
       real_num_txt++;
  
  if (real_num_txt)
  {
    printf("ANSWERS: (%d people entered some text)\n", real_num_txt);
    
    for (i = 0; i < num_txt; i++)
    {
      if (strlen(rpt->theData.txtAnswers.list[i]) > 2)
      {
        user_name = rpt->theData.txtAnswers.name[i];
        printf("\n== %s\n", user_name);
        print_indented(2, rpt->theData.txtAnswers.list[i]);
        printf("\n");
      }
    }
  }
}


void
print_results()
{
  cout << "\nSURVEY RESULTS\n-------------\n\n";
  ATYPE type;
  char *qtext;
  
  int i;
  

  for (i = 0; i < numQueries; i++)
  {
    type = all_queries[i]->getType();
    
    qtext = all_queries[i]->getname();
    printf("\nQuestion #%3d -- %s\n", i+1, qtext);
    delete qtext;
    
    printf("  %d user", all_reports[i]->num_answered);

    if (all_reports[i]->num_answered != 1)
      printf("s");

    printf(" answered this question, %d local, %d remote\n\n",
	   all_reports[i]->local_answered,
	   all_reports[i]->remote_answered );

    qtext = all_queries[i]->getquestion()->gettext();
    print_indented(2, qtext);
    printf("\n\n");
    
    delete qtext;
    
    if (type == CHOOSE)
      displayChoices(i);
    else if (type == TEXT)
      displayText(i); 

    displayComments(i);
  }
}


int
main(int argc, char **argv)
{
  SURVEY theSurvey;

  FILE   *fpSurvey, *fpResult;

  char   resultdir[256];

/*
** open up the survey from argv[1] or ./obj-survey if not specified
*/

  argc--;

  if (argc)
    fpSurvey = fopen(argv[1], "r");
  else
    fpSurvey = fopen("./obj-survey", "r");
  
  if (fpSurvey == NULL)
  {
    cerr << "Can't open survey file\n";
    exit(1);
  }

  argc--;
  
  sprintf(resultdir,"./");

  if (argc)
    DO_LOCAL_STUFF = 1;

/*
** read in the survey
*/
  
  theSurvey.readFromDisk(fpSurvey);
  numQueries = countQueries(&theSurvey, aq,
			    &numSections, QperSect);

  printf ("Read survey..%d queries, %d sections\n", numQueries, numSections);
  setup_reports();

  printf("Running the report - trying to read results data from: \n");
  printf("  directory:  %s\n", resultdir);
  run_report(&theSurvey, resultdir);

  print_results();
}
