#!/bin/make
#
# bitset - output a bitset given sorted integers on input
#
# @(#) $Revision$
# @(#) $Id$
# @(#) $Source$
#
# Copyright (c) 2001 by Landon Curt Noll.  All Rights Reserved.
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

SHELL= /bin/sh
CC= cc
#CFLAGS= -g3 -O2 -Wall
CFLAGS= -g3 -Wall

DESTDIR = /usr/local/bin
INSTALL= install
TARGETS= bitset

all: ${TARGETS}

bitset: bitset.o
	${CC} ${CFLAGS} bitset.o -o bitset

bitset.o: bitset.c
	${CC} ${CFLAGS} bitset.c -c

install: all
	${INSTALL} -m 0755 bitset ${DESTDIR}/bitset

clean:
	rm -f *.o

clobber: clean
	rm -f ${TARGETS}
