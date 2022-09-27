# Copyright 2022 Thomas E. Vaughan
# See LICENSE for terms of distribution.

CXX:=clang++
CC:= $(CXX)
CXXFLAGS:= -Wall -g -O0 -std=c++17
CPPFLAGS:= -I../include
LDLIBS:= -lCatch2Main -lCatch2

.PHONY: all clean

all: FillBiLinTest FillTest

FillTest: FillTest.o

FillBiLinTest: FillBiLinTest.o

clean:
	@rm -frv .d
	@rm -fv FillBiLinTest
	@rm -fv FillTest
	@rm -fv *.o

# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation
SRCS:= FillTest.cpp FillBiLinTest.cpp
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
