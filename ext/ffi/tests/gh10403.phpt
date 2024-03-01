--TEST--
GH-10403: Fix incorrect bitshifting and masking in ffi bitfield
--EXTENSIONS--
ffi
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) echo "skip this test is for 64-bit only"; ?>
--FILE--
<?php
$ffi = FFI::cdef(<<<EOF
    struct MyStruct {
        uint64_t x : 10;
        uint64_t y : 54;
    };
EOF);

$test_struct = $ffi->new('struct MyStruct');
$test_struct->x = 1023;
$test_values = [0x3fafbfcfdfefff, 0x01020304050607, 0, 0x3fffffffffffff, 0x2ffffffffffff5];
foreach ($test_values as $test_value) {
    $test_struct->y = $test_value;
    var_dump($test_struct->y === $test_value);
}
var_dump($test_struct->x);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(1023)
