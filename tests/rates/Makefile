ROOT_DIR=../..
SRC_DIR=${ROOT_DIR}/src
EXT_DIR=${ROOT_DIR}/external

CXX = clang++
INCLUDES=-I${SRC_DIR} -I${EXT_DIR}
CXXFLAGS=${INCLUDES} -std=c++23 -g
LDFLAGS=-L${SRC_DIR}/rates/bin -lrates

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
	$(BINDIR)/test_accrued \
	$(BINDIR)/test_deposit \
	$(BINDIR)/test_ir_future \
	$(BINDIR)/test_ir_swap_leg_fixed \
	$(BINDIR)/test_ir_swap_leg_floating \
	$(BINDIR)/test_ir_swap \
	$(BINDIR)/test_value \
	$(BINDIR)/test_yield_curve

test: all
	$(BINDIR)/test_accrued -s
	$(BINDIR)/test_deposit -s
	$(BINDIR)/test_ir_future -s
	$(BINDIR)/test_ir_swap_leg_fixed -s
	$(BINDIR)/test_ir_swap_leg_floating -s
	$(BINDIR)/test_ir_swap -s
	$(BINDIR)/test_value -s
	$(BINDIR)/test_yield_curve -s

$(BINDIR)/test_accrued: $(OBJDIR)/test_accrued.o
	$(LINK.cc) $(OBJDIR)/test_accrued.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_deposit: $(OBJDIR)/test_deposit.o
	$(LINK.cc) $(OBJDIR)/test_deposit.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_ir_future: $(OBJDIR)/test_ir_future.o
	$(LINK.cc) $(OBJDIR)/test_ir_future.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_ir_swap_leg_fixed: $(OBJDIR)/test_ir_swap_leg_fixed.o
	$(LINK.cc) $(OBJDIR)/test_ir_swap_leg_fixed.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_ir_swap_leg_floating: $(OBJDIR)/test_ir_swap_leg_floating.o
	$(LINK.cc) $(OBJDIR)/test_ir_swap_leg_floating.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_ir_swap: $(OBJDIR)/test_ir_swap.o
	$(LINK.cc) $(OBJDIR)/test_ir_swap.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_value: $(OBJDIR)/test_value.o
	$(LINK.cc) $(OBJDIR)/test_value.o $(LOADLIBES) $(LDLIBS) -o $@

$(BINDIR)/test_yield_curve: $(OBJDIR)/test_yield_curve.o
	$(LINK.cc) $(OBJDIR)/test_yield_curve.o $(LOADLIBES) $(LDLIBS) -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

-include $(DEP)
