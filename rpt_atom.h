// rpt_atom.h
// rpt atoms are data structures that contain statistical information
// for a particular question in a query.  The type of information that
// they contain is dependent on what type of answer the question it.
//
// If the question is:
//
//   o multiple choice
//     We count how many times each choice was selected.  We also will
//     indicate which answers came from remote users and which came from
//     local users.  This is a temporary hack until something more general
//     can be implemented.
//
//   o text or short-text
//     Simple; all we do is store a list of the answers.  A list of strings.
//
//   o integer or float
//     Simple; we store a list of these simple types.  Averaging, or any
//     other statistics can be done as post processing.  I should
//     eventually add support for these types of stats in the report
//     generator.
//
// Enough said, time to code...


#ifndef _RPT_ATOM_
#define _RPT_ATOM_

#include "answer.h"
#include <stream.h>

class ANSWER {
public:
  ANSWER();
  ~ANSWER();

// the data

private:

};

#endif

