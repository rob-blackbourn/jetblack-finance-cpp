ROOT_DIR=../..
SRC_DIR=${ROOT_DIR}/src
EXT_DIR=${ROOT_DIR}/externals
INSTALL_DIR=$(HOME)/.local/bin

CXX = clang++
INCLUDES = -I${SRC_DIR} -I${EXT_DIR} -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib
CXXFLAGS = -g -std=c++23 -Wall ${INCLUDES}
LDLIBS = ${LIBS} -lssl -lcrypto

LIBNAME = librates.a
OBJDIR = obj
BINDIR = bin

SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)
DEP = $(OBJ:$(OBJDIR)/%.o=$(OBJDIR)/%.d)
LIB = $(BINDIR)/$(LIBNAME)

SUFFIXES += .d
NODEPS := clean

$(OBJDIR)/%.d: %.cpp
	$(CPP) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) -MF $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: all
all: $(LIB)

$(LIB): $(BINDIR) $(OBJDIR) $(OBJ)
	$(AR) rcu $(LIB) $(OBJ)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) $(LIB)
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)

.PHONY: install
install: $(APP)
	cp $(APP) $(INSTALL_DIR)

.PHONY: uninstall
uninstall: $(APP)
	rm -f $(INSTALL_DIR)/$(LIBNAME)

-include $(DEP)
