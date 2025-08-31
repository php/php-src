--TEST--
Declaration Lists with Pointers
--EXTENSIONS--
ffi
--SKIPIF--
--FILE--
<?php
$ffi = FFI::cdef(<<<EOF
    struct MyStruct {
        uint8_t** a, *b, c;
    };
EOF);

$test_struct = $ffi->new('struct MyStruct');
$one = $ffi->new("uint8_t");
$oneptr = $ffi->new("uint8_t*");
$oneptrptr = $ffi->new("uint8_t**");
$one->cdata = 1;
$oneptr = FFI::addr($one);
$oneptrptr = FFI::addr($oneptr);

$test_struct->a = $oneptrptr;
$test_struct->b = $oneptr;
$test_struct->c = $one;

var_dump($test_struct->a[0][0]);
var_dump($test_struct->b[0]);
var_dump($test_struct->c);
?>
--EXPECT--
int(1)
int(1)
int(1)
