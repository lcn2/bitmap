/*
 * bitset - set bits in a bitmap given sorted integers on input
 *
 * usage:
 *	bitmap start step
 *
 *	start	starting value of the first bit in the bitmap
 *	step	difference in value between successive bits in the bitmap
 *
 * The purpose of this program is to set bits in a bitmap.  The input to
 * this program are integers indicating which bits in the bitmap to set.
 * The output of this program is the resulting bitmap.  The final octet
 * written is the highest octet in the bitmap with a set bit.
 *
 * NOTE: step must be an integer > 0
 *
 * NOTE: start, step and any input values be able to be represented as
 *	 signed long long.  On most machines where a long long is a 64
 *	 bit value, this means that those values must be >= -(2^63) and
 *	 <= (2^63 - 1).
 *
 * This program will convert integers read on stdin into a bitmap that
 * is written to stdout.  The integer input should be sorted because
 * any successor input value <= the previously non-ignored input value
 * will be ignored.  Input integers should be separated by newlines.
 *
 * Input values that are smaller than the start value are also ignored.
 *
 * Any sorted value that cannot be represented in the bitmap is ignored.  I.e.,
 * any input value that is != start % step will be ignored.  This is because
 * the bitmap that is written to stdout represents values as follows:
 *
 *	The bit value from octet 'x' and bit 'y', i.e.,:
 *
 *		(octet[x] & (1<<y))
 *
 *	represents the value:
 *
 *		start + step * (x*8 + y)
 *
 *	The value 'v', is represented by octet 'x' bit 'y':
 *
 *		t = (v - start) / step
 *		x = t / 8
 *		y = t % 8
 *
 *	provided that (v - start) % step == 0.  (i.e., the value
 *	is not ignored because it cannot be represented in the bitmap).
 *
 *	NOTE: 8 is used because there are 8 bits per octet (see OCTETBITS)
 *
 * One should note that if step is 1, then the bitmap represents the
 * integers beginning with start:
 *
 *	When step is 1, octet 'x' bit 'y' represents the value:
 *
 *		start + (x*8 + y)
 *
 * For prime bitmaps, the start is often 1 and the step is often 2.  Thus
 * the bitmap represents the odd positive integers:
 *
 *	When start==1, step==2, octet 'x' bit 'y' represents the value:
 *
 *		1 + 2*(x*8 + y) == 16*x + 2*y + 1
 *
 * For each non-ignored input value read on input, its corresponding bit is
 * set to 1.  All other bits are set to 0.  Thus the bitmap that is written
 * has 1's for non-ignored input values and 0's everywhere else.
 *
 * The length of the output will be determined by the highest (last)
 * non-ignored input value, relative to the start value.  The bitmap
 * is always written in whole octets, with the extra padding bits being
 * filled with zeros.  So it is possible that up to 7 extra 0 bits may be
 * written to stdout.
 *
 * The program operates on one bitmap buffer at a time.  The buffer
 * contains BUFSIZ octets where BUFSIZ is defined by <stdio.h>.
 * Frequently BUFSIZ is 8k octets.
 *
 * If the input is malformed (cannot be converted into a signed long long)
 * then an warning message will be sent to stderr.  If the input value is <
 * the previous non-ignored input value (unsorted), an warning message will
 * be sent to stderr.  However duplicate values, values < start and
 * values that cannot be represented in the bitmap (due to step) will
 * be silently ignored.
 */

/*
 * @(#) $Revision$
 * @(#) $Id$
 * @(#) $Source$
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
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <memory.h>
#include <stdlib.h>


/*
 * misc constants
 */
#define MAXLINE (19+1)	/* 2^63-1 is 19 digits long + newline */
#define OCTETBITS (8)	/* 8 bits per octet */

/*
 * a BUFSIZ chunk of the output bitmap, with an extra guard octet for safety
 *
 * This is the current bitmap buffer with a first bit value of 'bottom'.
 */
static u_int8_t buffer[BUFSIZ+1];

/*
 * Zero filled bitmap for when there are large gaps as we need to
 * output 0-filled buffers before setting the next bit.
 */
static u_int8_t zero[BUFSIZ+1];

/*
 * our name
 */
char *program;

int
main(int argc, char *argv[])
{
    int64_t start;		/* starting bitmap value */
    int64_t step;		/* bitmap increment value */
    int64_t value;		/* input value from stdin */
    int64_t line;		/* input line number */
    int64_t bottom;		/* low bit value of bitmap */
    int64_t span;		/* range of values spanned by a bitmap */
    int64_t beyond;		/* value of bit just beyond end of bitmap */
    int had_prev;		/* 1 ==> seen a previous non-ignored value */
    int64_t prev;		/* previous non-ignored value */
    int64_t boffset;		/* total bit offset in buffer for value */
    int octet;			/* octet offset in buffer for value */
    int bit;			/* bit offset in byte for value */
    char inbuf[MAXLINE+1];	/* max input line + newline + NUL byte */

    /*
     * parse args
     */
    program = argv[0];
    if (argc != 3) {
	fprintf(stderr, "usage: %s start step\n", program);
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
	fprintf(stderr, "%s: step: %lld must be > 0\n", program, step);
	exit(4);
    }

    /*
     * setup and initialize
     *
     * There is some deep magic in the beyond calculation.  It is possible
     * for the bitmap buffer to extend beyond the max (2^63-1) value.  If this
     * ever happens, then bottom > beyond.  This isn't a big problem because
     * at some point the input values will either overflow the range
     * or become unsorted.  Either was the non-ignored input will stop at
     * or before the highest possible bit value.
     */
    line = 0;	/* 1st line will be 1 */
    memset(inbuf, '\0', MAXLINE+1);
    memset(buffer, '\0', BUFSIZ+1);
    memset(zero, '\0', BUFSIZ+1);
    bottom = start;
    span = OCTETBITS*BUFSIZ*step;
    beyond = start + span;
    had_prev = 0;	/* no previous non-ignored value */
    prev = 0;

    /*
     * output sieve buffers until EOF
     */
    clearerr(stdin);
    while (fgets(inbuf, MAXLINE+1, stdin) != NULL) {
	char *p;	/* char check pointer */

	/*
	 * count this line
	 */
	++line;

	/*
	 * sanity check on input
	 *
	 * Input must be an integer (with a possible leading -)
	 * followed by a newline followed by a NUL.
	 */
	/* in case no newline was read */
	inbuf[MAXLINE] = '\0';
	/* leading - is OK */
	p = ((inbuf[0] == '-') ? inbuf+1 : inbuf);
	/* can only have digits followed by newline */
	while (*p != '\0' && isdigit(*p) && p < inbuf+MAXLINE) {
	    ++p;
	}
	/* we better have stopped on a newline followed by NUL */
	if (*p != '\n' || *(p+1) != '\0') {
	    /* improper line, ignore it */
	    if (*p == '\0') {
		fprintf(stderr, "%s: line %lld: ignoring, line too long\n",
			program, line);
	    } else {
		fprintf(stderr, "%s: line %lld: ignoring, invalid chars\n",
			program, line);
	    }
	    continue;
	}

    	/*
	 * convert input into an input value
	 */
	errno = 0;
	value = strtoll(inbuf, NULL, 0);
	if (errno == ERANGE) {
	    fprintf(stderr, "%s: line %lld: ignoring, value out of range\n",
		    program, line);
	    continue;
	}

	/*
	 * warn if unsorted, silently if equal
	 */
	if (had_prev && value <= prev) {
	    if (value < prev) {
		fprintf(stderr, "%s: line %lld: ignoring, value not sorted\n",
			program, line);
	    }
	    continue;
	}

	/*
	 * silently ignore if below start
	 */
	if (value < start) {
	    continue;
	}

	/*
	 * silently ignore if not a bitmap potential value
	 */
	if (((value - start) % step) != 0) {
	    continue;
	}

	/*
	 * At this point we know that the value will cause us to set a
	 * bit somewhere in the bitmap, the question is where.  It could
	 * be on the current bitmap.  It could be in some future bit map
	 * causing is to have to write this bitmap, followed by 0 or more
	 * 0-filled bitmaps before being able to set the bit in the new
	 * bitmap.
	 */

    	/*
	 * case: value is beyond current bitmap
	 *
	 * NOTE: We must check for beyond > bottom because the current
	 *	 bitmap buffer could go beyond 2^63-1.
	 */
    	if (beyond > bottom && value >= beyond) {

	    /*
	     * write the current bitmap buffer
	     */
	    clearerr(stdout);
	    if (fwrite(buffer, 1, BUFSIZ, stdout) != BUFSIZ) {
		fprintf(stderr, "%s: buffer write error: %s\n",
			program, strerror(errno));
		exit(5);
	    }

	    /*
	     * If there is a large gap, then we may need to write out
	     * 1 or more zero filled buffers.  In any event we must
	     * at least update the bitmap buffer 'bottom' and 'beyond' values.
	     */
	    do {
		/* update bitmap buffer range values */
		bottom = beyond;
		beyond += span;

		/*
		 * determine if 0-filled bitmap buffer needs to be written
		 *
		 * NOTE: We must check for beyond > bottom because the current
		 *	 bitmap buffer could go beyond 2^63-1.
		 */
		if (beyond > bottom && value >= beyond) {

		    /*
		     * write the 0-filled bitmap buffer
		     */
		    clearerr(stdout);
		    if (fwrite(zero, 1, BUFSIZ, stdout) != BUFSIZ) {
			fprintf(stderr, "%s: 0-buffer write error: %s\n",
				program, strerror(errno));
			exit(6);
		    }
		}
	    /* NOTE: beyond > bottom magic again */
	    } while(beyond > bottom && value >= beyond);

	    /*
	     * We have just written our older bitmap buffer, so we must zero it
	     * out for the new range to use.
	     */
	    memset(buffer, '\0', BUFSIZ+1);
	}

	/*
	 * At this point we know that we need to set a bit in the current
	 * bitmap buffer.  We will now determine where the bit to be set
	 * resides.
	 */
	boffset = (value - start) / step;
	/* firewall */
	if (boffset > (u_int64_t)BUFSIZ*OCTETBITS) {
	    fprintf(stderr, "%s: FATAL: unexpected bit offset: %lld > %d\n",
		    program, boffset, BUFSIZ*OCTETBITS);
	    exit(7);
	}
	octet = (int)(boffset / OCTETBITS);
	bit = (int)(boffset % OCTETBITS);

	/*
	 * Set the bit ... this is where the useful work is done!  :-)
	 */
	buffer[octet] |= (1<<bit);

	/*
	 * note that we have a (perhaps new) non-ignored previous value
	 */
    	had_prev = 1;
	prev = value;
	/* clear any error flags */
	clearerr(stdin);
    }

    /*
     * firewall - catch the case of stdin error
     */
    if (ferror(stdin)) {
	fprintf(stderr, "%s: read error: %s\n", program, strerror(errno));
	exit(8);
    }

    /*
     * We have reached the end of input, so it is time to output the
     * current and partial bitmap buffer.  It is possible that we did not find
     * any values, and thus the buffer will be empty.  It is also possible
     * that we will write only a few of the bitmap buffer octets as we
     * will stop writing at the last octet for which there is a 1 bit.
     */

    /*
     * determine the highest octet for which there is a 1 bit, if any
     */
    for (octet = BUFSIZ-1; octet >= 0 && buffer[octet] == 0; --octet) {
    }

    /*
     * write out only the bitmap octets that are needed, if any
     */
    if (octet >= 0) {
	clearerr(stdout);
	if (fwrite(buffer, 1, octet+1, stdout) != octet+1) {
	    fprintf(stderr, "%s: final buffer write error: %s\n",
		    program, strerror(errno));
	    exit(9);
	}
    }

    /*
     * All done!
     *
     * 	-- Jessica Noll, 1985
     */
    exit(0);
}
