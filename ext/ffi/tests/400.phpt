--TEST--
FFI 400: Call each other type struct of multiple FFI instance
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--INI--
ffi.enable=1
--FILE--
<?php
$ffi1 = FFI::cdef("
    typedef unsigned int time_t;
    typedef unsigned int suseconds_t;
    struct timeval {
        time_t      tv_sec;
        suseconds_t tv_usec;
    };
");
$ffi2 = FFI::cdef("
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
$tv = $ffi1->new("struct timeval");
$tz = $ffi2->new("struct timezone");
// call C's gettimeofday()
$r = $ffi2->gettimeofday(FFI::addr($tv), FFI::addr($tz));
var_dump($r);
?>
--EXPECT--
int(0)