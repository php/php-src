--TEST--
GH-9598: Fix use-after-free for nested FFI::addr() calls
--SKIPIF--
<?php
if (!extension_loaded('ffi')) die('skip ffi extension not available');
?>
--INI--
ffi.enable=1
--FILE--
<?php

$ffi = \FFI::cdef(<<<'CPP'
typedef struct {
    int8_t a;
} Example;
CPP);

$struct = $ffi->new('Example');
$structPtrPtr = \FFI::addr(\FFI::addr($struct));
var_dump($structPtrPtr[0][0]);

?>
--EXPECTF--
object(FFI\CData:struct <anonymous>)#6 (1) {
  ["a"]=>
  int(0)
}
