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
 * NOTE: start, step and any input values be able to be represented as 
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
 *	provided that (v - start) == 0 mod step.  (i.e., the value
 *		t = (v - start) / step
 *		x = t / 8
 *		y = t % 8
 *
 *	provided that (v - start) % step == 0.  (i.e., the value
 *	is not ignored because it cannot be represented in the bitmap).
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
 * filled with zeros.  So it is possible that up to 7 extra 0 bits may be 
 *
 * The length of the output will be determined by the highest (last)
 * non-ignored input value, relative to the start value.  The bitmap
 * is always written in whole octets, with the extra padding bits being
 * filled with zeros.  So it is possible that up to 7 extra 0 bits may be
 * then an warning message will be sent to stderr.  If the input value is <
 * the previous non-ignored input value (unsorted), an warning message will
 * be sent to stderr.  However duplicate values, values < start and
 * values that cannot be represented in the bitmap (due to step) will
 * be silently ignored.
#include <ctype.h>
#include <errno.h>


/*
 * maximum useful input line
static u_int8_t buffer[BUFSIZ+1];
u_int8_t bitmap[BUFSIZ+1];
/*

/*
 * our name
 */
char *program;

int
main(int argc, char *argv[])
    int64_t bottom;		/* low bit value of buffer */
    int64_t span;		/* range of values spanned by a buffer */
    int64_t beyond;		/* value of bit just beyond end of buffer */
    int had_prev;		/* 1 ==> seen a previous non-ignored value */
    int64_t prev;		/* previous non-ignored value */
    int64_t boffset;		/* total bit offset in buffer for value */
    int octet;			/* octet offset in buffer for value */
    int bit;			/* bit offset in byte for value */

	fprintf(stderr, "usage: %s start step\n", argv[0]);
	exit(0);
     */
    if (argc != 3) {
	fprintf(stderr, "%s: start value must be [-(2^63),(2^63-1)]\n",
    step = strtoll(argv[2], NULL, 0);
	fprintf(stderr, "%s: step: %lld must be > 0\n", argv[0], step);
	exit(2);
        exit(3);
    }
    if (step <= 0) {
	fprintf(stderr, "%s: step: %lld must be > 0\n", program, step);
	exit(4);
    }
     * for the buffer to extend beyond the max (2^63-1) value.  If this
     * ever happens, then bottom > beyond.  This isn't a big problem becase
     * setup and initialize
     *
     * There is some deep magic in the beyond calculation.  It is possible
     * for the bitmap buffer to extend beyond the max (2^63-1) value.  If this
     * ever happens, then bottom > beyond.  This isn't a big problem because
    memset(inbuf, '\0', BUFSIZ+1);
     */
    line = 0;	/* 1st line will be 1 */
    memset(inbuf, '\0', MAXLINE+1);
    bottom = start;
    span = 8*BUFSIZ*step;
    beyond = start + span;
    had_prev = 0;	/* no previous non-ignored value */
    while (fgets(inbuf, MAXLINE+1, stdin) == NULL) {
    /*
     * output sieve buffers until EOF
     */
    errno = 0;
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
			argv[0], line);
	if (*p != '\n' || *(p+1) != '\0') {
	    /* improper line, ignore it */
			argv[0], line);
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
		    argv[0], line);
	errno = 0;
	value = strtoll(inbuf, NULL, 0);
     * firewall - catch the case of stdin error
     *
     * 	-- Jessica Noll, 1985
     */
    exit(0);
}
