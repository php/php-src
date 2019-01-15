<?php
$ffi = FFI::cdef("

    int printf(const char *format, ...);
    const char * getenv(const char *);
    unsigned int time(unsigned int *);

    typedef unsigned int time_t;
    typedef unsigned int suseconds_t;

    struct timeval {
        time_t      tv_sec;
        suseconds_t tv_usec;
    };

    struct timezone {
        int tz_minuteswest;
        int tz_dsttime;
    };

	int gettimeofday(struct timeval *tv, struct timezone *tz);
    ", "libc.so.6");

var_dump($ffi->printf("Hello World from %s!\n", "PHP"));
var_dump($ffi->getenv("PATH"));
var_dump($ffi->time(null));

$tv = $ffi->new("struct timeval");
$tz = $ffi->new("struct timezone");
var_dump($ffi->gettimeofday(FFI::addr($tv), FFI::addr($tz)));
var_dump($tv, $tz);
