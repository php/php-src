--TEST--
GH-25837 (FFI: struct with an array field passed by value is described to libffi with a void element)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

$ffi = FFI::cdef(<<<EOF
    typedef struct {
        uint8_t numbers[2];
        uint8_t other;
    } test;

    typedef struct {
        test inner[1][2];
        uint8_t other;
    } wrapper;

    typedef struct {
        uint8_t (*read_first)(test x, uint8_t last);
        uint8_t (*read_second)(test x, uint8_t last);
        uint8_t (*read_third)(test x, uint8_t last);
        uint8_t (*read_first_wrapped)(wrapper x, uint8_t last);
    } functions;
    EOF,
);

$functions = $ffi->new('functions');

$functions->read_first = fn ($test, $last) => $test->numbers[0];
$functions->read_second = fn ($test, $last) => $test->numbers[1];
$functions->read_third = fn ($test, $last) => $last;
$functions->read_first_wrapped = fn ($test, $last) => $test->inner[0][0]->numbers[0];

$test = $ffi->new("test");
$test->numbers[0] = 1;
$test->numbers[1] = 2;
$test->other = 3;

var_dump(($functions->read_first)($test, 4));
var_dump(($functions->read_second)($test, 4));
var_dump(($functions->read_third)($test, 4));

$wrapper = $ffi->new('wrapper');
$wrapper->inner = $ffi->new("test[1][2]");
$wrapper->inner[0][0] = $test;
$wrapper->inner[0][1] = $ffi->new("test");
$wrapper->inner[0][1]->numbers[0] = 4;
$wrapper->inner[0][1]->numbers[1] = 5;
$wrapper->other = 5;

var_dump(($functions->read_first_wrapped)($wrapper, 7));

?>
--EXPECT--
int(1)
int(2)
int(4)
int(1)
