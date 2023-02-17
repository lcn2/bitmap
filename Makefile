#!/usr/bin/env make
#
# bitmap - bitmap operations
#
# Copyright (c) 2001,2023 by Landon Curt Noll.  All Rights Reserved.
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby granted,
# provided that the above copyright, this permission notice and text
# this comment, and the disclaimer below appear in all of the following:
#
#       supporting documentation
#       source copies
#       source works derived from this source
#       binaries derived from this source or from derived source
#
# LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
# EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
# USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#
# chongo <was here> /\oo/\
#
# Share and enjoy!

SHELL= bash
CC= cc
#CFLAGS= -g3 -O2 -Wall
#CFLAGS= -g3 -Wall
CFLAGS= -O3 -g3 -Wall
RM= rm

DESTDIR = /usr/local/bin
INSTALL= install
TARGETS= bitset popcnt listbit

all: ${TARGETS}

bitset: bitset.c
	${CC} ${CFLAGS} bitset.c -o bitset

popcnt: popcnt.c
	${CC} ${CFLAGS} popcnt.c -o popcnt

listbit: listbit.c
	${CC} ${CFLAGS} listbit.c -o listbit

install: all
	-@for i in ${TARGETS}; do \
	    if cmp -s $$i ${DESTDIR}/$$i; then \
	        :; \
	    else \
		echo "installing $$i"; \
		${INSTALL} -m 0755 $$i ${DESTDIR}/$$i; \
	    fi; \
	done

clean:
	${RM} -f *.o

clobber: clean
	${RM} -f ${TARGETS}
