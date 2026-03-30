
# Portable Makefile with aggressive optimization defaults.
# Override CC/CFLAGS/LDFLAGS on the make command line or environment.

CC ?= cc

# Simple detection for MSVC (cl) vs GCC/Clang. If you need MSVC support,
# invoke make with CC=cl and a suitable shell (or build with Visual Studio).
MSVC := $(findstring cl,$(CC))

ifeq ($(MSVC),cl)
	# MSVC-style defaults (user may override)
	CFLAGS ?= /O2 /GL /DNDEBUG
	LDFLAGS ?= /LTCG
	RM ?= del /Q
	COMPILE = $(CC) $(CFLAGS) /Fe:$@ $<
else
	# Aggressive GCC/Clang defaults aimed at maximum runtime speed on the
	# local machine. These are appropriate when building on each target
	# platform (not for producing portable binaries). Override via
	# `make CFLAGS="..."` if needed.
	CFLAGS ?= -O3 -Ofast -march=native -mtune=native -flto -funroll-loops \
					 -fomit-frame-pointer -ffast-math -fno-math-errno \
					 -fno-asynchronous-unwind-tables -fno-common -std=c11 -Wall -Wextra -pipe
	LDFLAGS ?= -flto -Wl,-O1
	RM ?= rm -f
	COMPILE = $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
endif

all: greg randgreg

greg: greg.c
	$(COMPILE)

randgreg: randgreg.c
	$(COMPILE)

test: greg
	./greg 1000

clean:
	$(RM) greg randgreg greg.txt

.PHONY: all test clean
