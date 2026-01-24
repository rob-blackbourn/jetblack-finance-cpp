ROOT_DIR=../..
SRC_DIR=${ROOT_DIR}/src
EXT_DIR=${ROOT_DIR}/external

CXX = clang++
INCLUDES=-I${SRC_DIR} -I${EXT_DIR}
CXXFLAGS=${INCLUDES} -std=c++23 -g

OBJDIR = obj
BINDIR = bin

SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)
DEP = $(OBJ:$(OBJDIR)/%.o=$(OBJDIR)/%.d)

SUFFIXES += .d
NODEPS := clean

$(OBJDIR)/%.d: %.cpp
	$(CPP) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) -MF $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: all clean test

all: \
	$(OBJDIR) $(BINDIR) \
	$(BINDIR)/test_match

test: all
	$(BINDIR)/test_match -s

$(BINDIR)/test_match: $(OBJDIR)/test_match.o
	$(LINK.cc) $(OBJDIR)/test_match.o $(LOADLIBES) $(LDLIBS) -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

-include $(DEP)
