# Copyright 2018-2022 Thomas E. Vaughan
# See LICENSE for terms of distribution.

CXXFLAGS = -Wall -g

# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation
SRCS = regfill.cpp
CC = $(CXX)

.PHONY: all clean

all : regfill.pdf regfill

# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation
regfill : regfill.o

regfill.pdf : regfill.tex trees-raw.pgm
	pdflatex $<
	pdflatex $<

trees-raw.pgm : trees.pgm
	pnmcat -lr trees.pgm > trees-raw.pgm

clean :
	@rm -fv *.log
	@rm -fv *.aux
	@rm -fv *.out
	@rm -fv *.o
	@rm -fv regfill
	@rm -fv regfill.pdf
	@rm -fv trees-raw.pgm
	@rm -fv mask.pgm
	@rm -fv noise.pgm
	@rm -fv s_mask.pgm
	@rm -fv t_mask.pgm
	@rm -fv trees-mod1.pgm
	@rm -fv trees-mod2.pgm
	@rm -fv trees-mod3.pgm


# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@
%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)
$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))

