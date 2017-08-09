/*
  helpBox.cc
  Dialog box with help info inside

  Due to time constraints, this is not generic yet  :-

*/

#include <curses.h>

extern void smart_box(WINDOW *, int, int, int, int);
// extern void beep();
extern "C" void sleep(int);

void
helpBox(WINDOW *parent, int LINES, int COLS) {

  /* create a new window
  ** determine the center of the screen
  ** and draw the dialog box.
  */

  int yp, xp, ht, wd;
  WINDOW *popWin;
  char ch;
  int done;

  ht = 15;
  wd = 60;
  
  yp = (LINES/2) - (ht/2);
  xp = (COLS/2)  - (wd/2);

  popWin = subwin(parent, ht, wd, yp-2, xp);
  wclear(popWin);
  
  wmove(popWin, 1, 20);
  wstandout(popWin);
  wprintw(popWin, "Survey Tool Help");
  wstandend(popWin);

  wmove(popWin, 3, 0);

  wprintw(popWin, "   Several options are available while in the survey  \n");
  wprintw(popWin, "   tool.  Below is a listing of what's available.  The\n");
  wprintw(popWin, "   C- means, hold down the control key.               \n");
  wprintw(popWin, "                                                      \n");
  wprintw(popWin, "   key    action                                      \n");
  wprintw(popWin, "   C-d    exit from a text window                     \n");
  wprintw(popWin, "   C-p    go back one question                        \n");
  wprintw(popWin, "   C-n    go forward one question                     \n");
/*
  wprintw(popWin, "   C-e    go to the last answered question            \n");
  wprintw(popWin, "   C-c    comment on the current question             \n");
*/
  smart_box(popWin, 0, 0, ht-1, wd-1 );

  wmove(popWin, 12, 14);
  wprintw(popWin, "Press the ");
  wstandout(popWin);
  wprintw(popWin, "space bar");
  wstandend(popWin);
  wprintw(popWin, " to continue");
  wmove(popWin, 12, 29);

  wrefresh(popWin);

  ch = wgetch(popWin);
  while (ch != ' ')
  {
    beep();
    ch = wgetch(popWin);
  }
  wclear(popWin);
  wrefresh(popWin);
  delwin(popWin);
  touchwin(parent);
}
