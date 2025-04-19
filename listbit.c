/*
 * listbit - list the positions of 0 or 1 bits
 *
 * We will read a bitmap from stdin and list the positions of either 0
 * or 1 bits.
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
 * Share and enjoy!  :-)
 */


#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>


/*
 * official version
 */
#define VERSION "1.8.1 2025-04-08"          /* format: major.minor YYYY-MM-DD */

/*
 * what we will count
 */
#define COUNT_ZERO (0)	/* count only 0 bits */
#define COUNT_ONE (1)	/* count only 1 bits */
#define OCTETBITS (8)	/* 8 bits per octet */


/*
 * usage message
 */
static const char * const usage =
  "usage: %s [-h] [-V] start step type\n"
        "\n"
        "    -h            print help message and exit\n"
        "    -V            print version string and exit\n"
        "\n"
        "    start         starting bitmap value\n"
        "    step          step values between bits\n"
        "    type          0 ==> list 0 bits, 1 ==> list 1 bits\n"
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


int
main(int argc, char *argv[])
{
    int cnttype;		/* what we will count */
    int readcnt;		/* chars read, or EOF */
    unsigned long start;	/* starting bitmap value */
    unsigned long step;		/* bitmap increment value */
    unsigned long value;	/* input value from stdin */
    int i;
    int j;

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
    if (argc != 3) {
        fprintf(stderr, "%s: ERROR: expected 3 args, found: %d\n", program, argc);
	fprintf(stderr, usage, program, prog, version);
        exit(3); /* ooo */
        /*NOTREACHED*/
    }

    /* parse start */
    errno = 0;
    start = strtoll(argv[0], NULL, 0);
    if (errno == ERANGE) {
	fprintf(stderr, "%s: failed to parse start value: %s\n", program, argv[0]);
	fprintf(stderr, usage, program, prog, version);
        exit(3); /* ooo */
        /*NOTREACHED*/
    }

    /* parse step */
    errno = 0;
    step = strtoll(argv[1], NULL, 0);
    if (errno == ERANGE) {
	fprintf(stderr, "%s: failed to parse step value: %s\n", program, argv[1]);
	fprintf(stderr, usage, program, prog, version);
        exit(3); /* ooo */
        /*NOTREACHED*/
    }
    if (step <= 0) {
	fprintf(stderr, "%s: step value must be > 0: %s\n", program, argv[1]);
	fprintf(stderr, usage, program, prog, version);
        exit(3); /* ooo */
        /*NOTREACHED*/
    }

    /* parse type */
    if (strcmp(argv[2], "0") == 0) {
	cnttype = COUNT_ZERO;
    } else if (strcmp(argv[2], "1") == 0) {
	cnttype = COUNT_ONE;
    } else {
	fprintf(stderr, "%s: count type: %s must be one of:\n"
	    "\t0 ==> count 0 bits\n"
	    "\t1 ==> count 1 bits\n", argv[2], program);
	fprintf(stderr, usage, program, prog, version);
        exit(3); /* ooo */
        /*NOTREACHED*/
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
     *	-- Jessica Noll, 1985
     */
    exit(0);
}
