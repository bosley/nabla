CC    ?= clang
CXX   ?= clang++

EXE = nhll.out

CDEBUG = -g -Wall

CXXDEBUG = -g -Wall

CSTD = -std=c99
CXXSTD = -std=c++17

CFLAGS = -Wno-deprecated-register -O0  $(CDEBUG) $(CSTD) 
CXXFLAGS = -Wno-deprecated-register -O0  $(CXXDEBUG) $(CXXSTD)


CPPOBJ = main nhll_driver
SOBJ =  parser lexer

FILES = $(addsuffix .cpp, $(CPPOBJ))

OBJS  = $(addsuffix .o, $(CPPOBJ))

CLEANLIST =  $(addsuffix .o, $(OBJ)) $(OBJS) \
				 nhll_parser.tab.cc nhll_parser.tab.hh \
				 location.hh position.hh \
			    stack.hh nhll_parser.output parser.o \
				 lexer.o nhll_lexer.yy.cc $(EXE)\

.PHONY: all
all: wc

wc: $(FILES)
	$(MAKE) $(SOBJ)
	$(MAKE) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) parser.o lexer.o $(LIBS)


parser: nhll_parser.yy
	bison -d -v nhll_parser.yy
	$(CXX) $(CXXFLAGS) -c -o parser.o nhll_parser.tab.cc

lexer: nhll_lexer.l
	flex --outfile=nhll_lexer.yy.cc  $<
	$(CXX)  $(CXXFLAGS) -c nhll_lexer.yy.cc -o lexer.o

.PHONY: clean
clean:
	rm -rf $(CLEANLIST)

