--TEST--
Bug #77632 (FFI Segfaults When Called With Variadics)
--SKIPIF--
<?php
require_once('skipif.inc');
try {
    $libc = FFI::cdef("int printf(const char *format, ...);", "libc.so.6");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$header = '
typedef struct _IO_FILE FILE;
extern FILE *stdout;
extern FILE *stdin;
extern FILE *stderr;

typedef uint64_t time_t;
typedef uint32_t pid_t;

time_t time(time_t*);
pid_t getpid(void);
int fprintf(FILE *, const char *, ...);
';

$ffi = FFI::cdef($header, 'libc.so.6');

try {
    $ffi->time();
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage() . "\n";
}

try {
    $ffi->time(null, null);
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage() . "\n";
}

try {
    $ffi->fprintf($ffi->stdout);
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage() . "\n";
}

try {
    $ffi->fprintf($ffi->stdout, 123, "Hello %s\n", "World");
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage() . "\n";
}
?>
--EXPECT--
FFI\Exception: Incorrect number of arguments for C function 'time', expecting exactly 1 parameter
FFI\Exception: Incorrect number of arguments for C function 'time', expecting exactly 1 parameter
FFI\Exception: Incorrect number of arguments for C function 'fprintf', expecting at least 2 parameters
FFI\Exception: Passing incompatible argument 2 of C function 'fprintf', expecting 'char*', found PHP 'int'
