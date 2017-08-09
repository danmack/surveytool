# survey tool Makefile

HDR         = survey.h section.h query.h question.h answer.h getput.h \
		response.h result.h
SRC         = survey.cc section.cc query.cc question.cc answer.cc getput.cc \
		response.cc result.cc box.cc dialog.cc countQueries.cc \
		helpBox.cc
TOOL_SRC    = srvtool.cc
REPORT_GEN  = rpgen.cc
CLASSES_OBJ = survey.o section.o query.o question.o answer.o getput.o \
		response.o result.o
TOOL_OBJ    = srvtool.o box.o dialog.o
S_OBJ	    = srvtool.o box.o dialog.o countQueries.o helpBox.o
REPORT_OBJ  = rpgen.o countQueries.o

# Standard/Required Targets and variables
SHELL      = /bin/sh
VERSION    = 0.1.0
CXX	   = g++
CXXFLAGS   = -g -O
DIST_FILES = ChangeLog IDEAS PROBLEMS Makefile $(SRC)

all: scc srv rpgen

scc: scc.y $(HDR) $(CLASSES_OBJ)
	flex -t scc.y > scc.cc
	$(CXX) -g scc.cc $(CLASSES_OBJ) -o scc -ll

srv:   $(S_OBJ) $(CLASSES_OBJ)
	$(CXX) -g $(S_OBJ) $(CLASSES_OBJ) -o srv -lcurses
	chmod 4755 srv

rpgen:	$(REPORT_OBJ) $(CLASSES_OBJ) $(HDR) $(SRC)
	$(CXX) -g $(REPORT_OBJ) $(CLASSES_OBJ) -o rpgen -lm

clean:
	-rm -f *.o
	-rm -rf scc.dSYM
	-rm -f scc srv stool scc.cc rpgen

