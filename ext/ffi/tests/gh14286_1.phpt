--TEST--
GH-14286 (ffi enum type (when enum has no name) make memory leak)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef("
    enum {
        TEST_ONE=1,
        TEST_TWO=2,
    };
    enum TestEnum {
        TEST_THREE=3,
    };
    struct TestStruct {
        enum {
            TEST_FOUR=4,
        } test1;
        enum TestEnum2 {
            TEST_FIVE=5,
        } test2;
    };
    typedef enum { TEST_SIX=6 } TestEnum3;
    struct {
        int x;
    };
    union {
        int x;
    };
");
var_dump($ffi->TEST_ONE);
var_dump($ffi->TEST_TWO);
var_dump($ffi->TEST_THREE);
var_dump($ffi->TEST_FOUR);
var_dump($ffi->TEST_FIVE);
var_dump($ffi->TEST_SIX);
?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
