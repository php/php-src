--TEST--
GH-25558 (FFI: struct with an enum field passed by value is described to libffi with a void element)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

$ffi = FFI::cdef(<<<EOF
    typedef enum {
        RED = 0,
        GREEN = 1
    } color;

    typedef struct {
        char a;
        color b;
        char c;
    } test;

    typedef struct {
        int (*read_b)(test x);
        char (*read_c)(test x);
    } functions;
    EOF,
);

$functions = $ffi->new('functions');

$functions->read_b = fn ($x) => $x->b;
$functions->read_c = fn ($x) => $x->c;

$test = $ffi->new('test');
$test->a = 2;
$test->b = $ffi->GREEN;
$test->c = 3;

var_dump(($functions->read_b)($test));
var_dump((int)($functions->read_c)($test));

?>
--EXPECT--
int(1)
int(3)
