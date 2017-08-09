# an ancient Multi User Survey Tool

This tool is a prototype multi user survey tool using a curses
interface meant to conduct surveys and collect the results after
which, the results would be aggregated, summarized, and analyzed.

The person in the conductor role would create the surveys in the survey
description language (included), compile the survey, and then deploy
it to hundreds to thousands of users who are able to mount the same
NFS server hierarchy where each user's results were written to individual
files indexed by their user-id (assumes flat user-id space).

This project served a purpose in 1992 but was only used in production
once.  For archeological reasons this has been resurrected and put
into GitHub.  There is a lot of bad programming in here for which I am
to blame but it does provide what I think is a cool survey description language
and a curses user interface allowing the user to convey choice, ranking, and
ther survey actions.

I will probably modernize this for fun the next time I am on sabbatical. The main
purpose of putting this on github is for preservation sake.

The initial github push builds and runs on Mac OS X Sierra/High
Sierra and with a little effort it has been used on IRIX, Linux,
SmartOS, and FreeBSD.  I am pretty sure it needs some C++ updates
for it to work with clang.

Enjoy,

Dan






