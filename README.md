# bitmap

* bitset - set bits in a bitmap given sorted integers on input

* listbit - list the positions of 0 or 1 bits

* popcnt - count the number of 0 or 1 bits of just bits


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
