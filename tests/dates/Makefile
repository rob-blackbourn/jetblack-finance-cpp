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
	$(BINDIR)/test_arithmetic \
	$(BINDIR)/test_business_days \
	$(BINDIR)/test_calendars \
	$(BINDIR)/test_daycount \
	$(BINDIR)/test_schedules

test: all
	$(BINDIR)/test_arithmetic -s
	$(BINDIR)/test_business_days -s
	$(BINDIR)/test_calendars -s
	$(BINDIR)/test_daycount -s
	$(BINDIR)/test_schedules -s

$(BINDIR)/test_arithmetic: $(OBJDIR)/test_arithmetic.o
	$(LINK.cc) $(OBJDIR)/test_arithmetic.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_business_days: $(OBJDIR)/test_business_days.o
	$(LINK.cc) $(OBJDIR)/test_business_days.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_calendars: $(OBJDIR)/test_calendars.o
	$(LINK.cc) $(OBJDIR)/test_calendars.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_daycount: $(OBJDIR)/test_daycount.o
	$(LINK.cc) $(OBJDIR)/test_daycount.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_schedules: $(OBJDIR)/test_schedules.o
	$(LINK.cc) $(OBJDIR)/test_schedules.o $(LOADLIBES) $(LDLIBS) -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

-include $(DEP)
