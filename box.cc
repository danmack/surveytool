#include <curses.h>

void
smart_box(WINDOW *win, int y, int x, int h, int w)
{
  char dash = '-';
  char bar  = '|';
  char plus = '+';
  int i;

  mvwaddch(win, y, x, plus);	               // upper left corner
  mvwaddch(win, h+(y-1), x, plus);	       // bottom left corner
  mvwaddch(win, h+(y-1), w+(x-1), plus);       // bottom right corner
  mvwaddch(win, y, w+(x-1), plus);	       // upper right corner


  // draw the horizontal lines of the box

  for ( i = x+1; i < (w+(x-1)); i++)
  {
    mvwaddch(win, y, i, dash);
    mvwaddch(win, h+(y-1), i, dash);
  }

  // draw the vertical lines of the box

  for ( i = y+1; i < (h+(y-1)); i++)
  {
    mvwaddch(win, i, x, bar);
    mvwaddch(win, i, w+(x-1), bar);
  }

}
