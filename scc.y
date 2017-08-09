 /*
 ** parse a Survey Description File and load it into
 ** a C++ SURVEY class, then dump the whole object to
 ** disk.
 */

  #include <stdio.h>
  #include "survey.h"

  char *pch, ch, *file_name;
  int num_lines = 0;
  int num_comments = 0;
  int num_errors = 0;
  int num_ch_printed = 0;
  int STATE = 0;
  FILE *testDataFile;

 /*
 ** need a static instantiation of survey
 ** to write out to disk
 */

  static SURVEY new_survey;

 /*
 ** pointers needed to build up the survey
 ** from its constituients
 */

  P_SECTION ptSsection;
  P_QUERY ptSquery;
  P_QUESTION ptSquestion;
  P_ANSWER ptSanswer;

 /*
 ** stand up and be prototyped
 */
     
 void parse_LP(char *yytext);
 void parse_RP(char *yytext);
 void parse_STRING(char *yytext);
 void parse_SURVEY(char *yytext);
 void parse_INTRO(char *yytext);
 void parse_QUERY(char *yytext);
 void parse_QUESTION(char *yytext);
 void parse_ANSWER(char *yytext);
 void parse_SELECTION(char *yytext);
 void parse_SHORT_TEXT(char *yytext);
 void parse_TEXT(char *yytext);
 void parse_EXCLUDE(char *yytext);
 void parse_COMMA(char *yytext);
 void parse_CHOOSE(char *yytext);
 void parse_RANK(char *yytext);
 void parse_INTEGER(char *yytext);
 void parse_FLOAT(char *yytext);
 void parse_ELIPSES(char *yytext);
 void parse_INT(char *yytext);
 void parse_SECTION(char *yytext);

 void add_newlines(char *, int);

 /*
 ** names and definitions
 */
LP		"("
RP		")"
SECTION         section|SECTION
STRING		\"[^"]*\"
SURVEY		survey|SURVEY
INTRO           intro|INTRO
QUERY		query|QUERY
QUESTION	question|QUESTION
ANSWER		answer|ANSWER
CHOOSE          CHOOSE|choose
RANK            RANK|rank
SHORT-TEXT	SHORT-TEXT|short-text
TEXT		TEXT|text
INTEGER         INTEGER|integer
FLOAT           FLOAT|float
EXCLUDE		exclude|EXCLUDE
ELIPSES         ".."
COMMA		,
INT             [0-9]+
%%
{LP}		parse_LP(yytext);
{RP}		parse_RP(yytext);
{SECTION}       parse_SECTION(yytext);
{STRING}	{ add_newlines(yytext, yyleng); parse_STRING(yytext); }
{SURVEY}	parse_SURVEY(yytext);
{INTRO}         parse_INTRO(yytext);  
{QUERY}		parse_QUERY(yytext);
{QUESTION}	parse_QUESTION(yytext);
{ANSWER}	parse_ANSWER(yytext);
{CHOOSE}        parse_CHOOSE(yytext);
{RANK}          parse_RANK(yytext);
{INTEGER}       parse_INTEGER(yytext);
{FLOAT}         parse_FLOAT(yytext);
{SHORT-TEXT}	parse_SHORT_TEXT(yytext);
{TEXT}		parse_TEXT(yytext);
{EXCLUDE}	parse_EXCLUDE(yytext);
{COMMA}		parse_COMMA(yytext);
{ELIPSES}       parse_ELIPSES(yytext);
{INT}           parse_INT(yytext);
^#.*\n		num_lines++;
[ \t]+                                                  /* eat white space */
\n              num_lines++;

.		{                                       /* unknown character */
  num_ch_printed = yyleng + strlen(file_name);
  printf("%s:%-5d:garbage >%s", file_name, num_lines+1, yytext);
  while ( (ch = yyinput()) != '\n' && ch != EOF )
  {
    if (num_ch_printed++ > (80 - (16+yyleng)))
      ; // don't print any more characters to keep things neater...
    else
      printf("%c", ch);
  }

  printf("\n");
  num_lines++;
  num_errors++;
 }
%%		
int
main(int argc, char *argv[])
{
  int survey_data;
  int status;


  switch(argc)
  {
  case 1:
    yyin = stdin;
    break;
  case 2:
    yyin = fopen(argv[1], "r");
    file_name = argv[1];
    break;
  default:
    printf("Usage: %s [surveyfile]\n", argv[0]);
  }

  printf("scc (version 0.9.5)\n");

  yylex();
  
  /*
  ** now we need to write the survey class out to disk
  ** the survey tool can then read in the structure
  */

  printf("compiled survey with %d lines, ", num_lines);

  if (num_errors == 0)
  {
    testDataFile = fopen("obj-survey", "w");
    new_survey.writeToDisk(testDataFile);
    fclose(testDataFile);

    printf("output is in obj-survey\n");
  }
  else
  {
    printf("no output written due to errors\n");
  }
}

void
stp_error(char *token)
{
  printf("%s:%-5d:token   '%s' not expected\n",
	 file_name, num_lines+1, token);
  exit(1);
}

void
add_newlines(char *s, int l)
{
  int i;

  // If there are any newlines in a matched strings, num_lines needs to
  // be adjusted so the compiler error messages make sense.

  for ( i = 0; i < l; i++)
    if (s[i] == 10)		// newline
      num_lines++;
}


void
parse_LP(char *yytext)
{
  switch(STATE)
  {
  case 0:
    STATE = 1;
    break;
  case 2:
    STATE = 3;
    break;
  case 4:
    STATE = 5;
    break;
  case 21:
    STATE = 25;
    break;
  case 22:
    STATE = 23;
    break;
  case 31:
    STATE = 32;
    break;
  case 33:
    STATE = 34;
    break;
  case 60:
    // if the user doesn't specify how many to choose, then
    // we assume the max and min choices are 1
    ptSanswer->setminch(1);
    ptSanswer->setmaxch(1);
    STATE = 61;
    break;
  case 63:
  case 64:
  case 65:
  case 66:
    STATE = 61;
    break;
  case 70:
    STATE = 71;
    break;
  case 80:
  case 83:
    STATE = 81;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_RP(char *yytext)
{
  switch(STATE)
  {
  case 16:
    STATE = 2;
    break;
  case 19:
    STATE = 2;
    break;
  case 21:
    // time to add another section to the survey object
    
    new_survey.add_section(ptSsection);

    STATE = 22;
    break;
  case 35:
    // at this point in time, our QUERY should be complete,
    // so we can go ahead and add it to the SECTION object
    // new_survey.add_query(ptSquery);

    ptSsection->add_query(ptSquery);
    
    STATE = 21;

    // hmm, I think we also need to free up the memory associated with
    // ptSquery, ptSquestion, and ptSanswer

    delete (ptSquestion);
//    delete (ptSquery);

//    delete (ptSanswer);

    break;
  case 46:
    // add the already obtained question to our query
    ptSquery->setquestion(ptSquestion);
    STATE = 33;
    break;
  case 51:
  case 52:
  case 53:
  case 54:
  case 63:
  case 83:
    // at this point in time, our ANSWER should have been complete
    // so we can go ahead and add it to our QUERY object.
    ptSquery->setanswer(ptSanswer);
    STATE = 35;
    break;
  case 62:
  case 67:
    STATE = 63;
    break;
  case 72:
    STATE = 67;
    break;
  case 82:
    STATE = 83;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_SECTION(char *yytext)
{
  switch(STATE)
  {
  case 3:
  case 23:
    STATE = 20;
    break;
  default:
    stp_error(yytext);
  }
}


void
parse_STRING(char *yytext)
{
  switch(STATE)
  {
  case 15:
    // store the survey's name
    yytext++;
    *(strrchr(yytext,'"')) = '\0';
    new_survey.setname(yytext);
    STATE = 16;
    break;
  case 18:
    yytext++;
    *(strrchr(yytext,'"')) = '\0';
    new_survey.setIntro(strdup(yytext));
    STATE = 19;
    break;
  case 20:
    yytext++;
    *(strrchr(yytext,'"')) = '\0';
    STATE = 21;
    // we have to add this section somehow to the survey
    ptSsection = new SECTION;
    ptSsection->setname(yytext);
    break;
  case 30:
    // create an instance of a query and name it
    STATE = 31;
    yytext++;
    *(strrchr(yytext,'"')) = '\0';
    ptSquery = new QUERY(yytext);
    break;
  case 45:
    // create an instance of a QUESTION, and store it's name
    STATE = 46;
    yytext++;
    *(strrchr(yytext,'"')) = '\0';
//    if (ptSquestion)
//      delete (ptSquestion);
    
    ptSquestion = new QUESTION(yytext);
    break;
  case 61:
    // add this choice to the current ANSWER object
    yytext++;
    printf("DEBUG [%s]\n", yytext);
    *(strrchr(yytext,'"')) = '\0';
    printf("DEBUG [%s]\n", yytext);
    printf("Calling addchoice [%s]\n", yytext);
    ptSanswer->addchoice(yytext);
    STATE = 62;
    break;
  case 71:
    // add this exclusion to the current choice of this ANSWER object
    yytext++;
    *(strrchr(yytext,'"')) = '\0';
    ptSanswer->addexclude(yytext);
    STATE = 72;
    break;
  case 81:
    // add this item to be ranked to this ANSWER object
    yytext++;
    *(strrchr(yytext,'"')) = '\0';
    ptSanswer->addchoice(yytext);
    STATE = 82;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_SURVEY(char *yytext)
{
  switch(STATE)
  {
  case 1:
    STATE = 15;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_INTRO(char *yytext)
{
  switch(STATE)
  {
  case 3:
    STATE = 18;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_QUERY(char *yytext)
{
  switch(STATE)
  {
  case 5:
  case 25:
    STATE = 30;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_QUESTION(char *yytext)
{
  switch(STATE)
  {
  case 32:
    STATE = 45;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_ANSWER(char *yytext)
{
  switch(STATE)
  {
  case 34:
    // create an instance of an ANSWER object
//    if (ptSanswer)
//      delete (ptSanswer);
    
    ptSanswer = new ANSWER;
    STATE = 50;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_CHOOSE(char *yytext)
{
  switch(STATE)
  {
  case 50:
    // set answer type to CHOOSE
    ptSanswer->settype(CHOOSE);
    STATE = 60;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_RANK(char *yytext)
{
  switch(STATE)
  {
  case 50:
    // set answer type to RANK
    ptSanswer->settype(RANK);
    STATE = 80;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_INTEGER(char *yytext)
{
  switch(STATE)
  {
  case 50:
    // set answer type to INTEGER
    ptSanswer->settype(INTEGER);
    STATE = 51;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_FLOAT(char *yytext)
{
  switch(STATE)
  {
  case 50:
    // set answer type to FLOAT
    ptSanswer->settype(FLOAT);
    STATE = 52;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_INT(char *yytext)
{
  switch(STATE)
  {
  case 60:
    // set the minimum/maximum number of choices to what yytext is
    ptSanswer->setminch(atoi(yytext));
    ptSanswer->setmaxch(atoi(yytext));
    STATE = 64;
    break;
  case 65:
    // set the maximum number of choices to what yytext is
    ptSanswer->setmaxch(atoi(yytext));
    STATE = 66;
    break;
  default:
     stp_error(yytext);
  }
}

void
parse_ELIPSES(char *yytext)
{
  switch(STATE)
  {
  case 64:
    // set max choices incase the user doesnt specify a max
    ptSanswer->setmaxch(MAX_CHOICES);
    STATE = 65;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_SHORT_TEXT(char *yytext)
{
  switch(STATE)
  {
  case 50:
    // set answer type to short text
    ptSanswer->settype(SHORT_TEXT);
    STATE = 54;
    break;
  default:
    stp_error(yytext);
  }
}

void
parse_TEXT(char *yytext)
{
  switch(STATE)
  {
  case 50:
    // set answer type to text
    ptSanswer->settype(TEXT);
    STATE = 53;
    break;
  default:
    stp_error(yytext);
  }	 
}

void
parse_EXCLUDE(char *yytext)
{
 switch(STATE)
 {
 case 62:
   STATE = 70;
   break;
 default:
   stp_error(yytext);
 }
}

void
parse_COMMA(char *yytext)
{
  switch(STATE)
  {
  case 72:
    STATE = 71;
    break;
  default:
    stp_error(yytext);
  }
}
