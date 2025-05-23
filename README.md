# bitmap

* bitset - set bits in a bitmap given sorted integers on input

> The purpose of this program is to set bits in a bitmap.  The input to
> this program are integers indicating which bits in the bitmap to set.
> The output of this program is the resulting bitmap.  The final octet
> written is the highest octet in the bitmap with a set bit.
>
> NOTE: step must be an integer > 0
>
> NOTE: start, step and any input values be able to be represented as
>       signed long long.  On most machines where a long long is a 64
>       bit value, this means that those values must be >= -(2^63) and
>       <= (2^63 - 1).
>
> This program will convert integers read on stdin into a bitmap that
> is written to stdout.  The integer input should be sorted because
> any successor input value <= the previously non-ignored input value
> will be ignored.  Input integers should be separated by newlines.
>
> Input values that are smaller than the start value are also ignored.
>
> Any sorted value that cannot be represented in the bitmap is ignored.  I.e.,
> any input value that is != start % step will be ignored.  This is because
> the bitmap that is written to stdout represents values as follows:
>
>      The bit value from octet 'x' and bit 'y', i.e.,:
>
>              (octet[x] & (1<<y))
>
>      represents the value:
>
>              start + step * (x*8 + y)
>
>      The value 'v', is represented by octet 'x' bit 'y':
>
>              t = (v - start) / step
>              x = t / 8
>              y = t % 8
>
>      provided that (v - start) % step == 0.  (i.e., the value
>      is not ignored because it cannot be represented in the bitmap).
>
>      NOTE: 8 is used because there are 8 bits per octet (see OCTETBITS)
>
> One should note that if step is 1, then the bitmap represents the
> integers beginning with start:
>
>      When step is 1, octet 'x' bit 'y' represents the value:
>
>              start + (x*8 + y)
>
> For prime bitmaps, the start is often 1 and the step is often 2.  Thus
> the bitmap represents the odd positive integers:
>
>      When start==1, step==2, octet 'x' bit 'y' represents the value:
>
>              1 + 2*(x*8 + y) == 16*x + 2*y + 1
>
> For each non-ignored input value read on input, its corresponding bit is
> set to 1.  All other bits are set to 0.  Thus the bitmap that is written
> has 1's for non-ignored input values and 0's everywhere else.
>
> The length of the output will be determined by the highest (last)
> non-ignored input value, relative to the start value.  The bitmap
> is always written in whole octets, with the extra padding bits being
> filled with zeros.  So it is possible that up to 7 extra 0 bits may be
> written to stdout.
>
> The program operates on one bitmap buffer at a time.  The buffer
> contains BUFSIZ octets where BUFSIZ is defined by <stdio.h>.
> Frequently BUFSIZ is 8k octets.
>
> If the input is malformed (cannot be converted into a signed long long)
> then an warning message will be sent to stderr.  If the input value is <
> the previous non-ignored input value (unsorted), an warning message will
> be sent to stderr.  However duplicate values, values < start and
> values that cannot be represented in the bitmap (due to step) will
> be silently ignored.

* listbit - list the positions of 0 or 1 bits

> We will read a bitmap from stdin and list the positions of either 0 or 1 bits.

* popcnt - count the number of 0 or 1 bits of just bits

> We will read a bitmap from stdin and count bits.  The count will be written to stdout.


# To install

```sh
make clobber all
sudo make install clobber
```


# To use


## bitset

```
/usr/localk/bin/bitset [-h] [-V] start step

    -h            print help message and exit
    -V            print version string and exit

    start	   starting bitmap value
    step	   step values between bits

Exit codes:
    0         all OK
    2         -h and help string printed or -V and version string printed
    3         command line error
 >= 10        internal error

bitset version: 1.8.1 2025-04-08
```


## listbit

```
/usr/local/bin/listbit [-h] [-V] start step type

    -h            print help message and exit
    -V            print version string and exit

    start         starting bitmap value
    step          step values between bits
    type          0 ==> list 0 bits, 1 ==> list 1 bits

Exit codes:
    0         all OK
    2         -h and help string printed or -V and version string printed
    3         command line error
 >= 10        internal error

listbit version: 1.8.1 2025-04-08
```


## popcnt

```
/usr/local/bin/popcnt [-h] [-V] type

    -h            print help message and exit
    -V            print version string and exit

    type          0 ==> count 0 bits, 1 ==> count 1 bits, 2 ==> count bits

Exit codes:
    0         all OK
    2         -h and help string printed or -V and version string printed
    3         command line error
 >= 10        internal error

popcnt version: 1.8.1 2025-04-08
```


# Reporting Security Issues

To report a security issue, please visit "[Reporting Security Issues](https://github.com/lcn2/bitmap/security/policy)".
