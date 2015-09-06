/*
 * popcnt - count the number of 0 or 1 bits of just bits
 *
 * usage:
 *	popcnt type
 *
 *	type	0 ==> count 0 bits, 1 ==> count 1 bits, 2 ==> count bits
 *
 * We will read a bitmap from stdin and count bits.  The count will
 * be wrtten to stdout.
 */

/*
 * @(#) $Revision: 1.2 $
 * @(#) $Id: popcnt.c,v 1.2 2001/12/28 18:29:10 chongo Exp root $
 * @(#) $Source: /usr/local/src/bin/bitmap/RCS/popcnt.c,v $
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
#define COUNT_ANY (2)	/* count any bit */
#define OCTETBITS (8)	/* 8 bits per octet */

/*
 * popcnt - popcnt[x] number of 1 bits in 0 <= x < 256
 */
char popcnt[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

/*
 * npopcnt - npopcnt[x] number of 0 bits in 0 <= x < 256
 */
char npopcnt[256] = {
    8, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 4,
    7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
    7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2, 2, 1,
    7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2, 2, 1,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2, 2, 1,
    5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2, 2, 1,
    4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0
};


int
main(int argc, char *argv[])
{
    int cnttype;	/* what we will count */
    int readcnt;	/* chars read, or EOF */
    u_int64_t bitcnt;	/* counted bits */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    if (argc != 2) {
	fprintf(stderr, "usage: %s type\n\n"
	    "\n"
	    "\ttype\t0 ==> count 0 bits\n"
	    "\t\t1 ==> count 1 bits\n"
	    "\t\t2 ==> count bits\n", program);
        exit(1);
    }
    if (strcmp(argv[1], "0") == 0) {
	cnttype = COUNT_ZERO;
    } else if (strcmp(argv[1], "1") == 0) {
	cnttype = COUNT_ONE;
    } else if (strcmp(argv[1], "2") == 0) {
	cnttype = COUNT_ANY;
    } else {
	fprintf(stderr, "%s: count type must be one of:\n"
	    "\t0 ==> count 0 bits\n"
	    "\t1 ==> count 1 bits\n"
	    "\t2 ==> count bits\n", program);
        exit(2);
    }

    /*
     * read buffers until EOF
     */
    bitcnt = 0;
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
	    exit(3);
	}

	/*
	 * count bits
	 */
	switch (cnttype) {
	case COUNT_ZERO:
	    for (i=0; i < readcnt; ++i) {
		bitcnt += npopcnt[buffer[i]];
	    }
	    break;
    	case COUNT_ONE:
	    for (i=0; i < readcnt; ++i) {
		bitcnt += popcnt[buffer[i]];
	    }
	    break;
    	case COUNT_ANY:
	    bitcnt += readcnt*OCTETBITS;
	    break;
        default:
	    fprintf(stderr, "%s: invalid cnttype: %d\n", program, cnttype);
	    exit(4);
	}
    } while (!feof(stdin));

    /*
     * report count
     */
    printf("%ld\n", bitcnt);

    /*
     * All done!
     *
     * 	-- Jessica Noll, 1985
     */
    exit(0);
}
