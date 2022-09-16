# File       Makefile
# Copyright  2022 Thomas E. Vaughan
# Brief      Top-level Makefile.

.PHONY: all clean

all:
	@$(MAKE) -C test

clean:
	@$(MAKE) -C test clean

# EOF
