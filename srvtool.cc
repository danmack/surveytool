// srvtool.cc

// include the survey class hierarchy

#include <iostream>
#include <curses.h>

#include "survey.h"
#include "queue.h"
#include "result.h"

// includes needed mostly for curses stuff

#define _BSD_SIGNALS 1
#include <fcntl.h>
#include <signal.h>
// #include <sgtty.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define NO_RESIZE    0
#define AFTER_RESIZE 1
#define ESCAPE_KEY   27
#define PREV_KEY     16
#define NEXT_KEY     14

/*
** prototypes to keep the compiler quiet
*/

extern "C" {
  int sleep(unsigned);
  int usleep(unsigned);
  void exit(int);
  int close(int);
  int getuid();
  // int read(int, char*, int);

#ifndef __sgi
#ifndef __linux
  // int printw(char *, ...);
  // int ioctl(int, int, caddr_t);
  int stty(int, struct sgttyb *);
#endif
#endif
  
// int getuid();
};

void adjust_window(int dummy);
void drawIntroScreen();
void drawQueryScreen();
int getAnswer();
void setToPrevious();
void setToNext();
void updateExcludes();
int  excluded_query();
int  excluded_section();
void moveAhead1();

extern void smart_box(WINDOW *, int, int, int, int);
extern char dialog(WINDOW *, int, int);
extern void helpBox(WINDOW *, int, int);
extern int countQueries(P_SURVEY, P_QUERY[10][50], int *, int *);

/*
** end of prototypes
*/

enum QSTATE {
  NORMAL, IN_INTRO, IN_HELP,
  IN_QUERY, IN_CHOOSER, IN_COMMENT,
  IN_TEXT, IN_RANKER, IN_SHORT_TEXT
};

char * COMMENTMSG = "Please enter your comment (press control-d to exit):";

/*
** An evil mess of global variables
*/

WINDOW *winMain;
WINDOW *winQuery;

int        curSectionNum, curQueryNum;
int        questionNumber = 1, maxQuestions;

char       result_file_name[256];
char      *result_file_dir = "/tmp";

P_SURVEY   theSurvey;
P_RESULT   theResults;
P_QUERY    allQueries[10][50];
P_QUERY    curQuery;
P_ANSWER   thisAnswer;
P_SECTION  thisSection;
P_RESPONSE thisResponse;
P_RESPONSE allResponses[128];

QSTATE     curState = NORMAL;
int        caughtResize = 0;
char      *thoseExcluded[128];
int        numExcluded = 0;

int        qpSection[20];
int        numSections;

int        PrevOrNext = 0;

/*
** end of globals
*/

struct sigvec sv_resize;

void
saveResults()
{
  FILE *fp;
  int i;
  int raw_loc = questionNumber;
  int tmpSnum, tmpQnum;

  theResults->setExcluded(numExcluded, thoseExcluded);

  /*
  ** add all of the responses so far to the RESULTS list
  ** and then write the data out to disk
  */

  tmpSnum = curSectionNum;
  tmpQnum = curQueryNum; 

  curSectionNum = 1;
  curQueryNum   = 1;
  theSurvey->gotoSection(curSectionNum);
  thisSection = theSurvey->get_current_section();
  thisSection->gotoQuery(curQueryNum);
  
  for (i =0; i < maxQuestions; i++)
  {
    if (!allResponses[i])
    {
      allResponses[i] = new RESPONSE(DUMMY);
    }
    else if (excluded_section() || excluded_query())
    {
      allResponses[i]->setType(DUMMY);
    }

    theResults->addResponse(allResponses[i], i);

    if (i < (maxQuestions-1))
      moveAhead1();
  }
  
  theResults->setBookmark(tmpSnum, tmpQnum, raw_loc);
  
  fp = fopen(result_file_name, "w");
  if (fp)
  {
    theResults->writeToDisk(fp);
    fclose(fp);
  }
}


int
drawPage(char *txt, int n)
{
  /*
  ** draw the all the text that will fit in the current screen
  ** return the last character printed so the calling function
  ** can use it as an index for the following page.
  */

  int i;
  int y, x;

  for (i = n; i < strlen(txt); i++)
  {
    printw("%c", txt[i]);
    getyx(winMain, y, x);
    if (y > (LINES - 4)) 
    {
      return i+1;
    }
  }
  return 0;
}


void
drawIntroScreen()
{
  char ch;
  char *introtext = theSurvey->getIntro();
  int i;
  int last_char;
  
  clear();

  last_char = 1;
  while (last_char)
  {
    clear();
    move(2, 0);
    
    if (last_char == 1)
      last_char = 0;
    
    last_char = drawPage(introtext, last_char);
    smart_box(winMain, 0, 0, LINES - 1, COLS);
    
    move(LINES-2, 27);
    printw("Press the ");
    standout();
    printw("space bar");
    standend();
    printw(" to continue.");
    move(LINES-2, 42);
    refresh();

    ch = wgetch(winMain);
    while (ch != 32)
    {
      if (ch == 8 || ch == 127 ||
	  ch == 'b' || ch == 'p' ||
	  ch == 'B' || ch == 'P' )
      {
	last_char = 1;
	ch = 32;
      }
      else
      {
	beep();
	ch = wgetch(winMain);
      }
    }
  }
  
  if (caughtResize)
  {
    caughtResize = 0;
    drawIntroScreen();
  }
}

void
drawInfoScreen()
{
  FILE *info_fp = fopen("./srv.info", "r");
  int result;
  int cnt, last_char;
  
  char ch, tmp;
  char infoText[8192];

  result = fscanf(info_fp, "%c", &tmp);
  cnt = 0;
  while (result != EOF)
  {
    infoText[cnt++] = tmp;
    result = fscanf(info_fp, "%c", &tmp);
  }
  fclose(info_fp);

  infoText[cnt] = NULL;
  last_char = 1;
  while (last_char)
  {
    clear();

    move(1, 0);

    if (last_char == 1)
      last_char = 0;

    last_char = drawPage(infoText, last_char);
    smart_box(winMain, 0, 0, LINES-1, COLS);

    move(LINES-2, 27);
    printw("Press the ");
    standout();
    printw("space bar");
    standend();
    printw(" to continue.");
    move(LINES-2, 42);
    refresh();

    ch = wgetch(winMain);
    while (ch != 32)
    {
      if (ch == 8 || ch == 127)
      {
	last_char = 1;
	ch = 32;
      }
      else
      {
	beep();
	ch = wgetch(winMain);
      }
    }
  }
  
  if (caughtResize)
  {
    caughtResize = 0;
    drawInfoScreen();
  }
}

void
drawThankyouScreen()
{
  FILE *thanks_fp = fopen("./srv.thanks", "r");
  int result;
  
  char ch, tmp;
  
  result = fscanf(thanks_fp, "%c", &tmp);

  printf("\n");
  
  while (result != EOF)
  {
    printf("%c", tmp);
    result = fscanf(thanks_fp, "%c", &tmp);
  }
  printf("\n");
  
  fclose(thanks_fp);
}

void
drawQueryScreen()
{
  char *qname, *qtext;
  int len;

  wclear(winQuery);
  clear();

  smart_box(winMain, 0, 0, LINES - 1, COLS);

  qname = thisSection->get_current_query_name();
  standout();
  move(0, 3);
  printw("%s", qname);

  move(0, 70);
  printw("%2d/%2d", questionNumber, maxQuestions);
  standend();

  wmove(winQuery, 1, 0);
  qtext = thisSection->get_current_question()->gettext();
  wprintw(winQuery, "%s", qtext);

  move(LINES - 2, 3);

  standout();
  standend();

  switch (curState)
  {
  case IN_CHOOSER:
    printw("Press ");
    standout();
    printw("ESC");
    standend();
    printw("ape for ");
    printw("HELP");
    break;
  case IN_TEXT:
    printw("Press ");
    standout();
    printw("ESC");
    standend();
    printw("ape for ");
    printw("HELP");
    move(LINES - 2, 54);
    printw("Press ");
    standout(); printw("control-d"); standend();
    printw(" to exit");
    break;
  case IN_SHORT_TEXT:
    printw("Please type in your response and press RETURN.");
    move(LINES - 1, 10);
    printw("Use Backspace/Delete to correct your mistakes.");
    break;
  case IN_RANKER:
    printw(
      "Press the digit corresponding to the highest ranked item, then the");
    move(LINES - 1, 10);
    printw(
      "2nd most highest, etc.  When all of the items have been ranked,");
    move(LINES - 0, 10);
    printw(
      "press the RETURN key.  Use Backspace/Delete to correct your mistakes.");
    break;
  default:
    exit(1);
  }

  refresh();
  wrefresh(winQuery);

  delete (qtext);
  delete (qname);
}


void
drawCommentScreen()
{
  char *qname;
  int len;

  wclear(winQuery);
  clear();

  smart_box(winMain, 0, 0, LINES - 1, COLS);

  qname = thisSection->get_current_query_name();
  len = strlen(qname);

  standout();
  move(0, 3);
  printw("%s", qname);
  standend();

  wmove(winQuery, 1, 0);
  wprintw(winQuery, "Please enter your comment (press control-d to exit):");

  move(LINES - 2, 3);
  printw("Press ");
  standout();
  printw("ESC");
  standend();
  printw("ape for HELP");

  refresh();
  wrefresh(winQuery);

  delete (qname);
}

void die(int code)
{
  signal(SIGINT, SIG_IGN);
  mvcur(0, COLS-1, LINES-1, 0);
  endwin();
  exit(0);
}

void
displayChoices(int y, int numChoices, P_tp_choice choices, int selected[])
{
  int i, l, j;
  int hON;
  
  char txt[1024];
  wmove(winQuery, y, 0);

  for (i = 0; i < numChoices; i++)
  {
    strcpy(txt, choices[i].text);
    l = strlen(txt);

    wprintw(winQuery, "  ");

    if (selected[i])
    {
      wstandout(winQuery);
      hON = 1;

      wprintw(winQuery, "%1d.  ", i+1);

      for (j = 0; j < l; j++)
      {
	if (txt[j] == 10)
	  hON = 0;
	else
	  if (txt[j] != ' ')
	    hON = 1;
      
	if (hON)
	  wstandout(winQuery);
	else
	  wstandend(winQuery);
	
	wprintw(winQuery, "%c", txt[j]);
      }
      wstandend(winQuery);
      hON = 0;
      wprintw(winQuery, "\n");
    }
    else
    {
      wprintw(winQuery, "%1d.  %s\n", i+1, txt);
    }

    if (selected[i])
      wstandend(winQuery);
  }
}

void
displayRankedItems(int y, int n, P_tp_choice choices,
		   int selected[], QUEUE<int> *rQueue)
{
  int i, j;
  
  int tx, ty;
  
  QUEUE<int> tQueue(MAX_CHOICES);

  int marked[MAX_CHOICES];

  for (i = 0; i < MAX_CHOICES; i++)
    marked[i] = 0;
  
  
  wmove(winQuery, y, 0);
  
  while ((j = rQueue->next()))
  {
    wprintw(winQuery, "  ");
    wstandout(winQuery);
    tQueue.add(j);
    wprintw(winQuery, "%1d.  %s\n", j, choices[j-1].text);
    marked[j-1] = 1;		// mark this one as already being printed
    wstandend(winQuery);    
  }

  for (i = 0; i < n; i++)
  {
    if (!marked[i])
      wprintw(winQuery, "  %1d.  %s\n", i+1, choices[i].text);
  }

  
  // put the original queue back together again
  while ((j = tQueue.next()))
    rQueue->add(j);
}

void
redrawTextScreen(char *header_text, char *iText)
{
  int i;
  wmove(winQuery, 0, 0);
  wprintw(winQuery, "\n%s\n\n", header_text);
  for (i = 0; i < strlen(iText); i++)
    wprintw(winQuery, "%c", iText[i]);
  wrefresh(winQuery);
}

char *
getText(char *header_text, char *prev_text)
{
  // get some lines of text from the user.

  int numChars = 0;
  int done     = 0;
  int xPos, yPos;
  int xMax, yMax;
  int xOrig, yOrig;

  int i;
  
  char ch;
  char iText[4096];

  // vars for doing the word wrap stuff

  char *end_of_last_word;
  char *tail_wrap;

  wmove(winQuery, 1, 0);
  wprintw(winQuery, "%s\n\n", header_text);
  wrefresh(winQuery);

  iText[0] = '\0';
  getyx(winQuery, yOrig, xOrig);

  if (prev_text)
  {
    numChars = strlen(prev_text);
    strcpy(iText, prev_text);
    redrawTextScreen(header_text, iText);
    getyx(winQuery, yPos, xPos);
  }

  while (!done)
  {
    ch = wgetch(winQuery);

    if (caughtResize)
    {
      caughtResize = 0;
      redrawTextScreen(header_text, iText);
    }

    getyx(winQuery, yPos, xPos);
    getmaxyx(winQuery, yMax, xMax);
    
    if ((ch > 31) && (ch < 127))         // printable character?
    {
      if (numChars < 4093)
      {
	/*
	** word wrap code
        */
	
	if (xPos == (xMax - 1))
	{
	  /*
	  ** insert a newline, and bring down the last word with us if
          ** necessary.
          */

	  if (ch != 32)
	  {
	    end_of_last_word = strrchr(iText, ' ');

	    if (end_of_last_word)
	    {
	      tail_wrap        = strdup(end_of_last_word+1);

	      *end_of_last_word++ = 10;
	      *end_of_last_word++ = 13;

	      while (*tail_wrap)
		*end_of_last_word++ = *tail_wrap++;

	      *end_of_last_word++ = ch;
	      *end_of_last_word   = '\0';
	      
	      numChars += 2;

	      wmove(winQuery, 1, 0);
	      wclrtobot(winQuery);
	      redrawTextScreen(header_text, iText);
	    }
	    else
	    {
	      iText[numChars++] = 10;
	      iText[numChars++] = 13;
	      iText[numChars++] = ch;
	      iText[numChars]   = '\0';
	      wprintw(winQuery, "\n%c", ch);
	      wrefresh(winQuery);
	    }
	  }
	  else
	  {
	    /*
            ** we ran into a space being typed in the last column.  Simply
	    ** add the lf/cr to string, and move the cursor to the next
            ** line.
	    */
	    iText[numChars++] = 10;
	    iText[numChars++] = 13;
	    iText[numChars] = '\0';
	    wprintw(winQuery, "\n");
	    wrefresh(winQuery);
	  }
	}
	else
	{
	  iText[numChars++] = ch;
	  iText[numChars]   = '\0';
      
	  wprintw(winQuery, "%c", ch);
	  wrefresh(winQuery);
	}
      }
      else
      {
	/*
	** No more space for characters, beep and disregard the character
	*/
	beep();
	wrefresh(winQuery);
      }
    }
    else if (ch == 13)
    {
      if (numChars < 4093)
      {
	iText[numChars++] = 10;
	iText[numChars++] = ch;
	iText[numChars]   = '\0';
      
	wprintw(winQuery, "\n");
	wrefresh(winQuery);
      }
      else
      {
	/*
	** No more space for characters, beep and disregard the character
	*/
	beep();
	wrefresh(winQuery);
      }
    }
    else if (ch == ESCAPE_KEY)
    {
      helpBox(winQuery, LINES, COLS);
      redrawTextScreen(header_text, iText);
      wrefresh(winQuery);
    }
    else if (ch == 4)				       // control d
    {
      return strdup(iText);
    }
    else if (ch == PREV_KEY)
    {
      PrevOrNext = -1;
      return strdup(iText);
    }
    else if (ch == NEXT_KEY)
    {
      PrevOrNext = 1;
      return strdup(iText);
    }
    else if ((ch == 8) || (ch == 127))		       // backspace/delete
    {
      if (numChars)
      {
	if (iText[--numChars] == 13)		       // a cr???
	{
	  iText[numChars]   = '\0';
	  iText[--numChars] = '\0';

	  wmove(winQuery, 1, 0);
	  wclrtobot(winQuery);
	  redrawTextScreen(header_text, iText);
	}
	else
	{
	  iText[numChars]   = '\0';
	  if ((xPos == 0) && (yPos > yOrig))
	  {
	    wmove(winQuery, yPos-1, (xMax - 1));
	    wdelch(winQuery);
	    wmove(winQuery, yPos-1, (xMax - 1));
	  }
	  else
	  {
	    wmove(winQuery, yPos, xPos-1);
	    wclrtoeol(winQuery);
	  }
	}
	wrefresh(winQuery);
      }
    }
    else if (ch == killchar())			       // kill line???
    {
      while (yPos - yOrig)
      {	
	wmove(winQuery, yPos--, 0);
	wclrtoeol(winQuery);
      }
      iText[0] = '\0';
      numChars = 0;

      wmove(winQuery, 1, 0);
      wclrtobot(winQuery);
      redrawTextScreen(header_text, iText);
    }
  }
  return (char *) 0;
}

char *
getShortText()
{
  // get some lines of text from the user.

  int numChars = 0;
  int numLines = 0;
  int done     = 0;
  int xPos, yPos;
  int xMax, yMax;
  int xOrig, yOrig;
  
  char ch;
  char iText[4096];
  iText[0] = '\0';

  wprintw(winQuery, "\n\nYour Answer? ");
  wrefresh(winQuery);
  
  getyx(winQuery, yOrig, xOrig);

  while (!done)
  {
    ch = wgetch(winQuery);

    if (caughtResize)
    {
      caughtResize = 0;
      wprintw(winQuery, "\n\nYour Answer? %s", iText);
      wrefresh(winQuery);
    }

    getyx(winQuery, yPos, xPos);
    getmaxyx(winQuery, yMax, xMax);
    
    if ((ch > 31) && (ch < 127))                       // printable character?
    {
      iText[numChars++] = ch;
      iText[numChars]   = '\0';
      wprintw(winQuery, "%c", ch);
      wrefresh(winQuery);
    }
    else if (ch == 13)				       // carriage return
    {
      return strdup(iText);
    }
    else if ((ch == 8) || (ch == 127))		       // backspace/delete
    {
      if (numChars)
      {
	iText[--numChars] = '\0';
	if ((xPos == 0) && (yPos > yOrig))
	{
	  wmove(winQuery, yPos-1, (xMax -1));
	}
	else
	{
	  wmove(winQuery, yPos, xPos-1);
	  wclrtoeol(winQuery);
	}
	wrefresh(winQuery);
      }
    }
    else if (ch == 21)
    {
      while (yPos - yOrig)
      {	
	wmove(winQuery, yPos--, 0);
	wclrtoeol(winQuery);
      }
      iText[0] = '\0';
      numChars = 0;
      wmove(winQuery, yOrig, xOrig);
      wclrtoeol(winQuery);
      wrefresh(winQuery);
    }
  }
  return (char *) 0;
}

void
ranker()
{
  int i, done, j;
  char ch;
  P_tp_choice choices = thisAnswer->getchoices();
  int numToRank       = thisAnswer->getnumch();
  int numRanked       = 0;
  
  int ox, oy;
  int xp, yp;

  int selected[MAX_CHOICES];
  int *copy_of_selected = new int[MAX_CHOICES];
  
  QUEUE<int> rQueue(MAX_CHOICES);
  QUEUE<int> tQueue(MAX_CHOICES);
  
  for (i = 0; i < MAX_CHOICES; i++)
    selected[i] = copy_of_selected[i] = 0;
  
  getyx(winQuery, oy, ox);

  displayRankedItems(oy+2, numToRank, choices, selected, &rQueue);
  wprintw(winQuery, "\nRank Highest to Lowest : ");
  wrefresh(winQuery);

  done = 0;
  while (!done)
  {
    ch = wgetch(winQuery);
    i  = ch - '1';

    if ((ch == 8) || (ch == 127))
    {
      i = rQueue.pop();
      if (i)
      {
	selected[i-1] = 0;
	numRanked--;
      }
      if (numRanked == (numToRank-1))
      {
	i = rQueue.pop();
	if (i)
	{
	  selected[i-1] = 0;
	  numRanked--;
	}
      }
    }
    else if (ch == 21)
    {
      while ((i = rQueue.next()))
	selected[i-1] = 0;
      numRanked = 0;
    }
    else if (ch == 13)
    {
      done = (numRanked == numToRank);
    }
    else if ((i >= 0) && (i < numToRank))
    {
      if (selected[i])
      {
	numRanked--;
	selected[i] = 0;

	while ((j = rQueue.next()))
	{
	  if (j != (i+1))
	    tQueue.add(j);
	}
	
	while ((i = tQueue.next()))
	  rQueue.add(i);

      }
      else if (numRanked < numToRank)
      {
	numRanked++;
	selected[i] = 1;
	rQueue.add(i+1);

	// if the user has ranked n-1 of the n items, we can automagically
	// select the last one for him/her.

	if (numRanked == (numToRank-1))
	  for (i = 0; i < numToRank; i++)
	    if (!selected[i])
	    {
	      numRanked++;
	      selected[i] = 1;
	      rQueue.add(i+1);
	    }
      }
    }

    if (caughtResize)
    {
      caughtResize = 0;
      getyx(winQuery, oy, ox);
      drawQueryScreen();
    }
    
    displayRankedItems(oy+2, numToRank, choices, selected, &rQueue);
    wprintw(winQuery, "\nRank Highest to Lowest : ");
    j = 0;
    while ((i = rQueue.next()))
    {
      wprintw(winQuery, "%d", i);
      j++;
      if (j < numToRank)
	wprintw(winQuery, ", ");
      
      tQueue.add(i);
    }
    
    while ((i = tQueue.next()))
      rQueue.add(i);
    
    wclrtoeol(winQuery);
    wrefresh(winQuery);
  } /* while !done */

  j = 0;
  while ((i = rQueue.next())) {
    copy_of_selected[j++] = i;
    tQueue.add(i);
  }

  thisResponse->setData(j, copy_of_selected);
}


int
chooser(int *prev_selected)
{
  int i, j;					       // temp iterator
  int done;					       // user is done
  char ch;					       // temp input char
  char *tmpText;
  int   tmpLength;
  P_tp_choice choices = thisAnswer->getchoices();
  int numChoices      = thisAnswer->getnumch();
  int minSelect       = thisAnswer->getminch();
  int maxSelect       = thisAnswer->getmaxch();
  int numSelected     = 0;
  
  int topXpos, topYpos;

  QUEUE<int> iQueue(MAX_CHOICES);
  QUEUE<int> tQueue(MAX_CHOICES);
  
  curState = IN_CHOOSER;

  getyx(winQuery, topYpos, topXpos);

  int selected[MAX_CHOICES];                           // used to keep track
						       // of what\'s selected
  int * copy_of_selected = new int[MAX_CHOICES];

/*
  display the choices available
*/

  if (!prev_selected)
    for ( i = 0; i < MAX_CHOICES; i++ )
      selected[i] = 0;
  else
    for ( i = 0; i < MAX_CHOICES; i++ )
    {
      selected[i] = prev_selected[i];
      if (selected[i])
      {
	numSelected++;
	selected[i] = 1;
	iQueue.add(i+1);
      }
    }

  // get input from the user, update selected/numSelected
  // and redisplay until the user hits return

  displayChoices(topYpos+2, numChoices, choices, selected);
  wprintw(winQuery, "\nYour Choice, (1-%d)? ", numChoices);

  j = 0;
  while ((i = iQueue.next()))
  {
    wprintw(winQuery, "%d", i);
    j++;
    if (j < maxSelect)
      wprintw(winQuery, ", ");
    
    tQueue.add(i);
  }
  
  while ((i = tQueue.next()))
    iQueue.add(i);
  
  wclrtoeol(winQuery);
  wrefresh(winQuery);
  
  done = 0;
  while (!done)
  {
    ch = wgetch(winQuery);
    i  = ch - '1';

    if ((ch == 8) || (ch == 127))
    {
      i = iQueue.pop();
      if (i) {
	selected[i-1] =  0;
	numSelected--;
      }
    }
    else if (ch == killchar())
    {
      // a control-u (kill-char) should erase all entries which means
      // draining the queue and zeroing the int array

      while ((i = iQueue.next()))
	selected[i-1] = 0;
      numSelected = 0;
    }
    else if (ch == ESCAPE_KEY)
    {
      helpBox(winQuery, LINES, COLS);
      drawQueryScreen();
      wrefresh(winQuery);
    }
    else if (ch == PREV_KEY)
    {
      for (i = 0; i < MAX_CHOICES; i++)
	copy_of_selected[i] = selected[i];
      thisResponse->setData(j, copy_of_selected);
      delete copy_of_selected;
      updateExcludes();
      setToPrevious();
      return -1;
    }
    else if (ch == NEXT_KEY)
    {
      for (i = 0; i < MAX_CHOICES; i++)
	copy_of_selected[i] = selected[i];
      thisResponse->setData(j, copy_of_selected);
      delete copy_of_selected;
      updateExcludes();
      setToNext();
      return 1;
    }
    else if (ch == 13)
    {
      done = (numSelected >= minSelect) && (numSelected <= maxSelect);
    }
    else if ((i >= 0) && (i < numChoices))
    {
      if (selected[i])
      {
	numSelected--;
	selected[i] = 0;

	while ((j = iQueue.next()))
	{
	  if (j != (i+1))
	    tQueue.add(j);
	}
	
	while ((i = tQueue.next()))
	  iQueue.add(i);

      }
      else if (numSelected < maxSelect)
      {
	numSelected++;
	selected[i] = 1;
	iQueue.add(i+1);
      }
    }
    else
    {
      if (!caughtResize)
	beep();
    }
    if (caughtResize)
    {
      caughtResize = 0;
      getyx(winQuery, topYpos, topXpos);
      drawQueryScreen();
    }
    
    displayChoices(topYpos+2, numChoices, choices, selected);
    wprintw(winQuery, "\nYour Choice, (1-%d)? ", numChoices);

    j = 0;
    while ((i = iQueue.next()))
    {
      wprintw(winQuery, "%d", i);
      j++;
      if (j < maxSelect)
	wprintw(winQuery, ", ");
      
      tQueue.add(i);
    }
    
    while ((i = tQueue.next()))
      iQueue.add(i);
    
    wclrtoeol(winQuery);
    wrefresh(winQuery);
  }
  
  // at this point in time, we should update the exclude list based on
  // what the user has selected.  This is fairly straght forward.

  for (i = 0; i < MAX_CHOICES; i++)
    if (selected[i])
    {
      for (j = 0; j < choices[i].num_excludes; j++)
      {
	thoseExcluded[numExcluded] =
	  strdup(choices[i].excludes[j]);
	numExcluded++;
      }
    }

  j = 0;

  for (i = 0; i < MAX_CHOICES; i++)
    copy_of_selected[i] = selected[i];
  
  thisResponse->setData(j, copy_of_selected);
  delete copy_of_selected;
  return 0;
}

void
saveQuit()
{
  curSectionNum = theSurvey->getNumCurSection();
  curQueryNum   = thisSection->getNumCurQuery();

  saveResults();
  clear();
  wrefresh(winQuery);
  refresh();
  endwin();

  printf("Your answers to the survey so far have been saved.  When you are\n");
  printf("ready to continue taking the survey, just restart with the command");
  printf("\n");
  printf("'survey' as you did the first time.   You will be placed back\n");
  printf("in at the point where you stopped.\n\n");

  printf("If you have any questions or problems with the tool, please\n");
  printf("call the help desk for assistance\n\n");
  printf("Thank you for participating in our survey.\n\n");

  exit(0);
}

int
getAnswer()
{
  ATYPE queryType;
  char * foo;
  char ch;
  int done = 0, save = 0;
  int tx, ty;
  char *qtext;
  int  *orig_choices = (int *) 0;
  char *orig_text    = (char *) 0;
  char *existing_comment = (char *) 0;
  

  
  thisAnswer = thisSection->get_current_answer();
  queryType = thisAnswer->get_atype();

  /*
  ** only allocate a new response object if we need one
  */

  if (allResponses[questionNumber-1] == 0)
    thisResponse = allResponses[questionNumber-1] = new RESPONSE(queryType);
  else
  {
    thisResponse = allResponses[questionNumber-1];
    existing_comment = thisResponse->getComment();
    if (queryType == CHOOSE)
    {
      orig_choices = thisResponse->getChoiceRankData();
    }
    else if (queryType == TEXT)
    {
      orig_text = thisResponse->getTextData();
    }
  }

  thisResponse->setType(queryType);
  
  while (!done) {
    
    switch (queryType)
    {
    case CHOOSE:
      curState = IN_CHOOSER;
      drawQueryScreen();

      if (chooser(orig_choices))
      {
	return 0;
      }
      
      break;
    case TEXT:
      curState = IN_TEXT;
      drawQueryScreen();
      qtext = thisSection->get_current_question()->gettext();
      thisResponse->setData(getText(qtext, orig_text));
      delete qtext;

      if (PrevOrNext)
      {
	updateExcludes();
	if (PrevOrNext == -1)
	  setToPrevious();
	else
	  setToNext();
	
	PrevOrNext = 0;
	return 0;
      }
      
      break;
    case SHORT_TEXT:
      curState = IN_SHORT_TEXT;
      drawQueryScreen();
      thisResponse->setData(getShortText());
      
      if (PrevOrNext)
      {
	updateExcludes();
	if (PrevOrNext == -1)
	  setToPrevious();
	else
	  setToNext();
	
	PrevOrNext = 0;
	return 0;
      }

      break;
    case RANK:
      curState = IN_RANKER;
      drawQueryScreen();
      ranker();
      break;
    default:
      ;
    }

/*     At this point in time, the user has finished entering something for
       the current question.  Pop a dialog box prompting the user for what
       he or she would like to do next.
*/
    
    ch = dialog(winQuery, LINES, COLS);

    switch (ch)
    {
    case 13:
    case 10:
      done = 1;
      break;
    case 'c':
    case 'C':
      drawCommentScreen();
      /*
      ** has the user already typed in a comment for this question?
      */

      thisResponse->
	setComment(
                   getText(COMMENTMSG, existing_comment));
      
      done = 1;
      break;
    case 'r':
    case 'R':
      break;
    case 's':
    case 'S':
      saveQuit();
      break;
    default:
      beep();
      break;
    }
  }
  
  if (questionNumber == maxQuestions)
    return 1;
  else
  {
    updateExcludes();
    setToNext();
    return 0;
  }
}

//
// adjust_window - called to adjust our window after getting a SIGWINCH
//

void adjust_window(int dummy)
{
  struct winsize w;
  if (ioctl(fileno(stdout), TIOCGWINSZ, (char *)&w) == 0 && w.ws_row > 0)
    LINES = w.ws_row;
  if (ioctl(fileno(stdout), TIOCGWINSZ, (char *)&w) == 0 && w.ws_col > 0)
    COLS= w.ws_col;

  // get out of curses first

  endwin();
  delete(winMain);
  
  // then reset the screen

  winMain = initscr();

  if (winQuery)
  {
    delete(winQuery);
    winQuery = newwin( LINES - 3, COLS - 4, 1, 2);
    scrollok(winQuery, TRUE);
  }

  cbreak();
  noecho();
  nonl();

  switch (curState)
  {
  case IN_CHOOSER:
    clear();
    wclear(winQuery);
    wrefresh(winQuery);
    caughtResize = 1;
    drawQueryScreen();
    break;
  case IN_INTRO:
  case IN_HELP:
  case IN_COMMENT:
    caughtResize = 1;
    break;
  default:
    caughtResize = 1;
    drawQueryScreen();
  }
}

int
sect_excluded(int n)
{
  int i;
  char *sname;
  
  theSurvey->gotoSection(n);
  sname = theSurvey->get_current_section_name();
  
  for ( i = 0; i < numExcluded; i++ )
    if (strcmp(sname, thoseExcluded[i]) == 0)
      {
	delete sname;
	return 1;
      }

  delete sname;
  return 0;
}

int
query_excluded(int s, int q)
{
  int i;
  char *qname;
  
  qname = allQueries[s][q]->getname();
  
  for ( i = 0; i < numExcluded; i++ )
    if (strcmp(qname, thoseExcluded[i]) == 0)
      {
	delete qname;
	return 1;
      }

  delete qname;
  return 0;
}


void
updateExcludes()
{
  int rn, sn, qn;
  P_RESPONSE rp;
  P_QUERY tq;
  P_tp_choice pch;
  int i, j;
  
  int *choyces;

  numExcluded = 0;
  
  rn = 0;
  for (sn = 0; sn < numSections; sn++)
  {
    if (sect_excluded(sn+1))
    {
      for (qn = 0; qn < qpSection[sn]; qn++)
      {
	rn++;
      }
    }
    else
      for (qn = 0; qn < qpSection[sn]; qn++)
      {
	if (query_excluded(sn, qn))
	{
	  ;
	}
	else
	{
	  if (allResponses[rn])
	  {
	    if (allResponses[rn]->getType() == CHOOSE)
	    {
	      choyces = allResponses[rn]->getChoiceRankData();
	      tq = allQueries[sn][qn];
	      pch= tq->getanswer()->getchoices();
	      
	      for (i = 0; i < 10; i++)
		if (choyces[i])
		{
		  for (j = 0; j < pch[i].num_excludes; j++)
		    thoseExcluded[numExcluded++] =
		      strdup(pch[i].excludes[j]);
		}
	    }
	  }
	}
	rn++;
      }
  }
}


//
// if the current query name has been excluded,
// return 1, else return 0
//

int
excluded_query()
{
  int i;

  for ( i = 0; i < numExcluded; i++ )
    if (strcmp(curQuery->getname(), thoseExcluded[i]) == 0)
      return 1;
  
  return 0;
}

//
// if the current query name has been excluded,
// return 1, else return 0
//

int
excluded_section()
{
  char * curSection = thisSection->getname();
  int i;

  for ( i = 0; i < numExcluded; i++ )
    if (strcmp(curSection, thoseExcluded[i]) == 0)
    {
      delete (curSection);
      return 1;
    }

  delete (curSection);
  return 0;
}

int
is_continuing()
{
  FILE *fp_results;
  int uid = getuid();
  int i, bad_input = 1, mid_done;
  char ch;
  char *temp_text;
  
  struct stat buf;
  char **saved_excludes;
  
  sprintf(result_file_name, "%s/%d", result_file_dir, uid);
  
  // check to see if this file already exists

  int exists = stat(result_file_name, &buf);

  if (exists == 0) 		// the file already exists, catch up time...
  {
    // get the number of answered questions from the result structure
    
    fp_results = fopen(result_file_name, "r");
    theResults->readFromDisk(fp_results);
    fclose(fp_results);

    // bring the exclude list back up to its previous state

    numExcluded    = theResults->getNumExcluded();
    saved_excludes = theResults->getExcludeList();

    for ( i = 0; i < numExcluded; i++)
      thoseExcluded[i] = saved_excludes[i];

    /* bring the allResponses array back up to speed as well */

    i = theResults->setTo1stResponse();
    while ((allResponses[i++] = theResults->getNextResponse()) != P_RESPONSE
	   (-1));

    /* tell the user that we are starting them off where they left off */
    /* or tell them that they've already finished taking it.           */

    clear();

    if (theResults->getStatus() == NOT_COMPLETED)
    {
      move(LINES/2 - 4, 2);
      printw("Welcome back to the survey!");
      move(LINES/2 - 2, 2);
      printw("You began the survey once but stopped midway through.  You may now");
      move(LINES/2 - 1, 2);
      printw("proceed to complete the survey and you will be placed back at the");
      move(LINES/2 - 0, 2);
      printw("point where you left off (all previous answers have been saved).");
      move(LINES/2 + 2, 2);
      printw("Do you wish to continue with the survey at this time (y/n)? ");

      refresh();
      ch = wgetch(winMain);
      mid_done = 0;
      while (!mid_done)
      {
	if (ch == 'y' || ch == 'Y')
	  mid_done = 1;
	else if (ch =='n' || ch == 'N')
	{
	  mid_done = 1;
	  clear();
	  refresh();
	  endwin();
	  drawThankyouScreen();
	  exit(0);
	}
	else
	{
	  beep();
	  ch = wgetch(winMain);
	}
      }
    }
    else
    {
      clear();
      
      move(LINES/2 - 4, 2);
      printw("Welcome back to the survey!");
      move(LINES/2 - 2, 2);
      printw("You have already completed the survey, however, it is possible to");
      move(LINES/2 - 1, 2);
      printw("re-take it.   If you wish to re-take the survey, your original answers");
      move(LINES/2 - 0, 2);
      printw("will be erased.");
      move(LINES/2 + 2, 2);
      printw("Do you wish to erase your original answers and re-take");
      move(LINES/2 + 3, 2);
      printw("the survey (y/n)? ");
      
      refresh();
      while (bad_input)
      {
	ch = wgetch(winMain);
	if (ch == 'y' || ch == 'Y')
	{
	  addch(ch);
	  refresh();
	  for (i = 0; i < numExcluded; i++)
	  {
	    delete (thoseExcluded[i]);
	  }
	  numExcluded = 0;

	  delete (theResults);
	  temp_text = theSurvey->getname();
	  theResults = new RESULT(temp_text, maxQuestions);
	  delete temp_text;


	  for (i =0; i < maxQuestions; i++)
	  {
	    allResponses[i] = (P_RESPONSE) 0;
	  }
	  return 0;
	}
	else if (ch == 'n' || ch == 'N')
	{
	  addch('n');
	  refresh();
	  sleep(1);
	  clear();
	  refresh();
	  endwin();
	  drawThankyouScreen();
	  exit(0);
	}
	beep();
      }
    }
    return 1;
  }
  return 0;
}

void
moveBack1()
{
  curQueryNum--;
  questionNumber--;
  
  if (curQueryNum < 1)
  {
    if (curSectionNum > 1)
    {
      curSectionNum--;
      curQueryNum = qpSection[curSectionNum-1];
    }
    else
    {
      beep();
      curQueryNum = 1;
      curSectionNum = 1;
      questionNumber = 1;
    }
  }

  curQuery = allQueries[curSectionNum-1][curQueryNum-1];
  theSurvey->gotoSection(curSectionNum);
  thisSection = theSurvey->get_current_section();
  thisSection->gotoQuery(curQueryNum);
  
  thisResponse = allResponses[questionNumber-1];
}


void
moveAhead1()
{
  curQueryNum++;
  questionNumber++;
  
  if (curQueryNum > qpSection[curSectionNum-1])
  {
    if (curSectionNum < theSurvey->getNumSections())
    {
      curSectionNum++;
      curQueryNum = 1;
    }
    else
    {
      beep();
      curQueryNum--;
      questionNumber = maxQuestions;
    }
  }

  curQuery = allQueries[curSectionNum-1][curQueryNum-1];
  theSurvey->gotoSection(curSectionNum);
  thisSection = theSurvey->get_current_section();
  thisResponse = allResponses[questionNumber-1];
  thisSection->gotoQuery(curQueryNum);
}

void
setToPrevious()
{
  /*
  ** given:
  **   curSectionNum, curQueryNum,
  **   questionNumber, maxQuestions,
  **   allQueries, and allResponses;
  **
  ** change the state of the program so that we are at the last
  ** question the user entered.
  */

  int thereYet = 0;

  while (!thereYet)
  {
    moveBack1();
    if (excluded_section() || excluded_query())
      ;
    else
      if (thisResponse)
	if (thisResponse->getType() != DUMMY)
	  thereYet = 1;
  }
}

void
setToNext()
{
  /*
  ** move forward through the survey, skipping exlcluded queries
  ** and handling the section boundary conditions
  */

  int thereYet = 0;
  while (!thereYet)
  {
    moveAhead1();
    if (excluded_section() || excluded_query())
      ;
    else
      thereYet = 1;
  }
}


int
main(int argc, char *argv[], char *environ[])
{
  // initialize the global curses stuff and conduct
  // the survey

  // srv will attempt to open compiled survey in current
  // directory - default name is obj-survey

  FILE *fp;
  int i, j, done;
  char *temp_text;

  int section_result, query_result;
  
  void die(int code);

  switch (argc)
  {
  case 3:
    delete result_file_dir;
    result_file_dir = argv[2];
    fp = fopen(argv[1], "r");
    break;
  case 2:
    fp = fopen(argv[1], "r");
    break;
  case 1:
    fp = fopen("./obj-survey", "r");
    break;
  default:
    fprintf(stderr, "Usage: srv [object-file [result-directory]]\n");
    exit (1);
  }

  if (fp == NULL) {
    perror("ERROR: cannot find the survey: ");
    exit(1);
  }
  
  theSurvey = new (SURVEY);
  theSurvey->readFromDisk(fp);
  fclose(fp);

  /*
  ** store each survey query in the 2d array
  */

  maxQuestions = countQueries(theSurvey, allQueries,
			      &numSections, qpSection);
  
  temp_text = theSurvey->getname();
  theResults = new RESULT(temp_text, maxQuestions);
  delete temp_text;

  // make curses happen, initialize, verify that the user has enough
  // rows/cols to handle the survey tool, if not, let them know and exit.

  winMain = initscr();
  winQuery = newwin( LINES - 3, COLS - 4, 1, 2 ); // help gap
  scrollok(winQuery, TRUE);

  if (((COLS) < 80) || ((LINES) < 24))
  {
    perror("screen needs to be at least 80x24");
    die(0);
  }

  for (i =0; i < 128; i++)
    allResponses[i] = (P_RESPONSE) 0;

  // facist I/O control commands

  cbreak();  noecho();  nonl();
//  raw();
  
  // tell curses what to do in case of a control-c and
  // what to do with with resize signals

  signal(SIGINT, die);

  sv_resize.sv_handler = adjust_window;
  sv_resize.sv_mask    = 0;
  sv_resize.sv_flags   = SV_INTERRUPT;

  (void) sigvec(SIGWINCH, &sv_resize, (struct sigvec *)NULL);

//  signal(SIGWINCH, adjust_window);
  
  // clear the screen and begin

  // find out whether or not this user has already taken/started the
  // survey.

  if (!is_continuing())
  {
    curState = IN_INTRO;
    drawIntroScreen();

    curState = IN_HELP;
    drawInfoScreen();
    section_result = theSurvey->set_to_first_section();
    thisSection    = theSurvey->get_current_section();
    query_result   = thisSection->set_to_first_query();
    curSectionNum  = 1;
    curQueryNum    = 1;
    curQuery       = allQueries[curSectionNum-1][curQueryNum-1];
  }
  else
  {
    curState = IN_INTRO;
    drawIntroScreen();

    section_result = 0;
    theSurvey->gotoSection(theResults->getLastSectionNum());
    thisSection    = theSurvey->get_current_section();
    questionNumber = theResults->getNumResponses();
    curSectionNum  = theResults->getLastSectionNum();
    curQueryNum    = theResults->getLastQueryNum();
    curQuery       = allQueries[curSectionNum-1][curQueryNum-1];
    if (thisSection)
    {
      thisSection->gotoQuery(theResults->getLastQueryNum());
    }
    else
    {
      ;
    }

  }

  clear();
  refresh();

  // loop until all sections have been selected

  done = 0;
  while (!done)
  {
    curState = IN_QUERY;
    done = getAnswer();
  }
  
  curState = NORMAL;

  // add the excludes to the results and write out the results to a file

  theResults->markCompleted();  // tell the file that the user is done
 
  saveResults();

  // say goodbye and exit

  clear();

  refresh();
  endwin();

  drawThankyouScreen();
}
