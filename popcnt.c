/*
 * popcnt - count the number of 0 or 1 bits of just bits
 *
 * We will read a bitmap from stdin and count bits.  The count will
 * be written to stdout.
 *
 * Copyright (c) 2001,2015,2023,2025 by Landon Curt Noll.  All Rights Reserved.
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
 * chongo (Landon Curt Noll) /\oo/\
 *
 * http://www.isthe.com/chongo/index.html
 * https://github.com/lcn2
 *
 * Share and Enjoy!     :-)
 */


#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>


/*
 * official version
 */
#define VERSION "1.8.1 2025-04-08"          /* format: major.minor YYYY-MM-DD */

/*
 * what we will count
 */
#define COUNT_ZERO (0)	/* count only 0 bits */
#define COUNT_ONE (1)	/* count only 1 bits */
#define COUNT_ANY (2)	/* count any bit */
#define OCTETBITS (8)	/* 8 bits per octet */


/*
 * usage message
 */
static const char * const usage =
  "usage: %s [-h] [-V] type\n"
        "\n"
        "    -h            print help message and exit\n"
        "    -V            print version string and exit\n"
        "\n"
	"    type          0 ==> count 0 bits, 1 ==> count 1 bits, 2 ==> count bits\n"
        "\n"
        "Exit codes:\n"
        "    0         all OK\n"
        "    2         -h and help string printed or -V and version string printed\n"
        "    3         command line error\n"
        " >= 10        internal error\n"
        "\n"
        "%s version: %s\n";


/*
 * static declarations
 */
static char *program = NULL;    /* our name */
static char *prog = NULL;       /* basename of program */
static const char * const version = VERSION;

/*
 * read buffer
 */
static u_int8_t buffer[BUFSIZ];

/*
 * popcnt - popcnt[x] number of 1 bits in 0 <= x < 256
 */
static const char popcnt[256] = {
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
static const char npopcnt[256] = {
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
    int cnttype;	    /* what we will count */
    int readcnt;	    /* chars read, or EOF */
    unsigned long bitcnt;   /* counted bits */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    prog = rindex(program, '/');
    if (prog == NULL) {
        prog = program;
    } else {
        ++prog;
    }
    while ((i = getopt(argc, argv, ":hV")) != -1) {
	switch (i) {

        case 'h':                   /* -h - print help message and exit */
	    fprintf(stderr, usage, program, prog, version);
            exit(2); /* ooo */
            /*NOTREACHED*/

	case 'V':                   /* -V - print version string and exit */
            (void) printf("%s\n", version);
            exit(2); /* ooo */
            /*NOTREACHED*/

	case ':':
            (void) fprintf(stderr, "%s: ERROR: requires an argument -- %c\n", program, optopt);
	    fprintf(stderr, usage, program, prog, version);
            exit(3); /* ooo */
            /*NOTREACHED*/

        case '?':
            (void) fprintf(stderr, "%s: ERROR: illegal option -- %c\n", program, optopt);
	    fprintf(stderr, usage, program, prog, version);
            exit(3); /* ooo */
            /*NOTREACHED*/

        default:
            fprintf(stderr, "%s: ERROR: invalid -flag\n", program);
	    fprintf(stderr, usage, program, prog, version);
            exit(3); /* ooo */
            /*NOTREACHED*/
        }
    }
    /* skip over command line options */
    argv += optind;
    argc -= optind;
    /* check the arg count */
    if (argc != 1) {
        fprintf(stderr, "%s: ERROR: expected 1 arg, found: %d\n", program, argc);
	fprintf(stderr, usage, program, prog, version);
        exit(3); /* ooo */
        /*NOTREACHED*/
    }

    /*
     * parse arg
     */
    if (strcmp(argv[0], "0") == 0) {
	cnttype = COUNT_ZERO;
    } else if (strcmp(argv[0], "1") == 0) {
	cnttype = COUNT_ONE;
    } else if (strcmp(argv[0], "2") == 0) {
	cnttype = COUNT_ANY;
    } else {
	fprintf(stderr, "%s: count type: %s must be one of:\n"
	    "\t0 ==> count 0 bits\n"
	    "\t1 ==> count 1 bits\n"
	    "\t2 ==> count bits\n", argv[0], program);
	fprintf(stderr, usage, program, prog, version);
        exit(3); /* ooo */
        /*NOTREACHED*/
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
     *	-- Jessica Noll, 1985
     */
    exit(0);
}
