/*
  dialog.cc
  Dialog box widget for curses.

  Due to time constraints, this is not generic yet  :-
*/

/* #include "dialog.h" */

#include <curses.h>

extern void smart_box(WINDOW *, int, int, int, int);
// extern void beep();
extern "C" void sleep(int);

char
dialog(WINDOW *parent, int LINES, int COLS) {

  /* create a new window
  ** determine the center of the screen
  ** and draw the dialog box.
  */

  int yp, xp, ht, wd;
  WINDOW *popWin;
  char ch;
  int done;

  ht = 13;
  wd = 48;

  yp = (LINES/2) - (ht/2);
  xp = (COLS/2)  - (wd/2);

  popWin = subwin(parent, ht, wd, yp-2, xp);
  wclear(popWin);

  wmove(popWin, 1, 15);
  wstandout(popWin);
  wprintw(popWin, "Survey Tool Options");
  wstandend(popWin);

  wmove(popWin, 3, 3);
  wprintw(popWin, "press");
  wmove(popWin, 4, 3);
  wprintw(popWin, "-----");
  wmove(popWin, 3, 20);
  wprintw(popWin, "to do this");
  wmove(popWin, 4, 20);
  wprintw(popWin, "----------");

  wmove(popWin, 5, 0);
  wprintw(popWin, "   return   To go on to the next question\n");
  wprintw(popWin, "     c      To comment on this question\n");
  wprintw(popWin, "     r      To redo this question\n");
  wprintw(popWin, "     s      To save your answers and quit\n\n");

  smart_box(popWin, 0, 0, ht-1, wd-1 );

  wmove(popWin, 10, 3);
  wprintw(popWin, "Your choice ? ");

  wrefresh(popWin);

  done = 0;
  ch = wgetch(popWin);
  while (!done)
  {
    done = 1;
    switch (ch)
    {
    case 13:
    case 10:
      break;
    case 'c':
    case 'C':
    case 'r':
    case 'R':
      wprintw(popWin, "%c", ch);
      break;
    case 's':
    case 'S':
      /*
      ** verify that the user really wants to save quit.
	*/
      wmove(popWin, 10, 3);
      wprintw(popWin, "Really save/quit (y/N)? ");
      wrefresh(popWin);

      ch = wgetch(popWin);

      if (ch == 'y' || ch == 'Y')
	ch = 's';
      else
      {
	done = 0;
	wmove(popWin, 10, 3);
	wprintw(popWin, "                        ");
	wmove(popWin, 10, 3);
	wprintw(popWin, "Your Choice? ");
	wrefresh(popWin);
	ch = 'n';
      }
      break;
    default:
      beep();
      done = 0;
      ch = wgetch(popWin);
    }
  }
  return ch;
}
