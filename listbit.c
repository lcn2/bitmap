/*
 * listbit - list the positions of 0 or 1 bits
 *
 * usage:
 *	listbit start step type
 *
 *	start	starting value of the first bit in the bitmap
 *	step	difference in value between successive bits in the bitmap
 *	type	0 ==> list 0 bits, 1 ==> list 1 bits
 *
 * We will read a bitmap from stdin and list the positions of either 0
 * or 1 bits.
 */

/*
 * @(#) $Revision: 1.2 $
 * @(#) $Id: listbit.c,v 1.2 2015/09/06 06:35:06 root Exp $
 * @(#) $Source: /usr/local/src/bin/bitmap/RCS/listbit.c,v $
 *
 * Copyright (c) 2001 by Landon Curt Noll.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo <was here> /\oo/\
 *
 * Share and enjoy!
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
 * our name
 */
char *program;

/*
 * read buffer
 */
static u_int8_t buffer[BUFSIZ];

/*
 * what we will count
 */
#define COUNT_ZERO (0)	/* count only 0 bits */
#define COUNT_ONE (1)	/* count only 1 bits */
#define OCTETBITS (8)	/* 8 bits per octet */


int
main(int argc, char *argv[])
{
    int cnttype;		/* what we will count */
    int readcnt;		/* chars read, or EOF */
    int64_t start;		/* starting bitmap value */
    int64_t step;		/* bitmap increment value */
    int64_t value;		/* input value from stdin */
    int i;
    int j;

    /*
     * parse args
     */
    program = argv[0];
    if (argc != 4) {
	fprintf(stderr, "usage: %s start step type\n"
	    "\n"
	    "\tstart\tstarting bitmap value\n"
	    "\tstep\tstep values between bits\n"
	    "\ttype\t0 ==> list 0 bits\n"
	    "\t\t1 ==> list 1 bits\n", program);
	exit(1);
    }
    errno = 0;
    start = strtoll(argv[1], NULL, 0);
    if (errno == ERANGE) {
	fprintf(stderr, "%s: start value must be [-(2^63),(2^63-1)]\n",
		program);
        exit(2);
    }
    errno = 0;
    step = strtoll(argv[2], NULL, 0);
    if (errno == ERANGE) {
	fprintf(stderr, "%s: start value must be [1,(2^63-1)]\n", program);
        exit(3);
    }
    if (step <= 0) {
	fprintf(stderr, "%s: step: %ld must be > 0\n", program, step);
	exit(4);
    }
    if (strcmp(argv[1], "0") == 0) {
	cnttype = COUNT_ZERO;
    } else if (strcmp(argv[1], "1") == 0) {
	cnttype = COUNT_ONE;
    } else {
	fprintf(stderr, "%s: count type must be one of:\n"
	    "\t0 ==> count 0 bits\n"
	    "\t1 ==> count 1 bits\n", program);
        exit(5);
    }

    /*
     * setup and initialize
     */
    value = start;

    /*
     * read buffers until EOF
     */
    do {

	/*
	 * read a buffer
	 */
	clearerr(stdin);
	readcnt = fread(buffer, 1, BUFSIZ, stdin);
	if (readcnt == EOF) {
	    break;	/* EOF found */
	} else if (readcnt <= 0 || ferror(stdin)) {
	    fprintf(stderr, "%s: read error: %s\n", program, strerror(errno));
	    exit(6);
	}

	/*
	 * print bits
	 */
	switch (cnttype) {
	case COUNT_ZERO:
	    for (i=0; i < readcnt; ++i) {
		if (buffer[i]) {
		    for (j=0; j < OCTETBITS; ++j) {
			if ((buffer[i] & (1<<j)) == 0) {
			    printf("%ld\n", value);
			}
			value += step;
		    }
		} else {
		    value += OCTETBITS*step;
		}
	    }
	    break;

    	case COUNT_ONE:
	    for (i=0; i < readcnt; ++i) {
		if (buffer[i]) {
		    for (j=0; j < OCTETBITS; ++j) {
			if ((buffer[i] & (1<<j)) != 0) {
			    printf("%ld\n", value);
			}
			value += step;
		    }
		} else {
		    value += OCTETBITS*step;
		}
	    }
	    break;

        default:
	    fprintf(stderr, "%s: invalid cnttype: %d\n", program, cnttype);
	    exit(7);
	}

    } while (!feof(stdin));

    /*
     * All done!
     *
     * 	-- Jessica Noll, 1985
     */
    exit(0);
}
