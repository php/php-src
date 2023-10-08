--TEST--
FFI Cannot nest FFI::addr() calls
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = \FFI::cdef(<<<'CPP'
typedef struct {
    int8_t bar;
} Foo;
CPP);

$struct = $ffi->new('Foo');
$structPtrPtr = \FFI::addr(\FFI::addr($struct));
?>
--EXPECTF--
Fatal error: Uncaught FFI\Exception: FFI::addr() cannot create a reference to a temporary pointer in %s:%d
Stack trace:
#0 %s(%d): FFI::addr(Object(FFI\CData:struct <anonymous>*))
#1 {main}
  thrown in %s on line %d
