# Copyright 2022 Thomas E. Vaughan
# See LICENSE for terms of distribution.

CXX:=clang++
CC:= $(CXX)
CXXFLAGS:= -Wall -g -O0 -std=c++17
CPPFLAGS:= -I../include
LDLIBS:= -lCatch2Main -lCatch2

.PHONY: all clean

all: FillBiLinTest FillTest interpolateTest bin2x2Test

FillTest: FillTest.o

FillBiLinTest: FillBiLinTest.o

interpolateTest: interpolateTest.o

bin2x2Test: bin2x2Test.o

clean:
	@rm -frv .d
	@rm -fv bin2x2Test
	@rm -fv FillBiLinTest
	@rm -fv FillTest
	@rm -fv interpolateTest
	@rm -fv *.o

# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation
SRCS:= FillTest.cpp FillBiLinTest.cpp interpolateTest.cpp bin2x2Test.cpp
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
