# Copyright 2022 Thomas E. Vaughan
# See LICENSE for terms of distribution.

CC:= $(CXX)
CXXFLAGS:= -Wall -g -O3 -std=c++17
CPPFLAGS:= -I../include
LDLIBS:= -lCatch2Main -lCatch2

.PHONY: all clean

all: FillExperimentTest

FillTest: FillTest.o

FillExperimentTest: FillExperimentTest.o

clean:
	@rm -frv .d
	@rm -fv *.o
	@rm -fv utest

# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation
SRCS:= FillTest.cpp FillExperimentTest.cpp
DEPDIR= .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS= -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
COMPILE.cc= $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE= @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@
%.o: %.cpp
%.o: %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)
$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))

# EOF
