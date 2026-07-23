--TEST--
GH-12910 ([FFI] Segfault/memory leak on incorrect use of FFI definitions)
--EXTENSIONS--
ffi
--SKIPIF--
<?php
require_once('utils.inc');
try {
    ffi_cdef("extern void *zend_printf;", ffi_get_php_dll_name());
} catch (Throwable $e) {
    die('skip PHP symbols not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php

require_once('utils.inc');

class MyClass
{
    public static function do(): void
    {
        // The printf signature is a lie for easier testing
        $ffi = FFI::cdef("struct struct1{int x;};struct struct2{int y;};void zend_printf(struct struct1*);", ffi_get_php_dll_name());
        $struct2 = $ffi->new("struct struct2");
        $ffi->zend_printf(FFI::addr($struct2));
    }
}

MyClass::do();

?>
--EXPECTF--
Fatal error: Uncaught FFI\Exception: Passing incompatible argument 1 of C function 'zend_printf', expecting 'struct struct1*', found 'struct struct2*' in %s:%d
Stack trace:
#0 %s(%d): FFI->zend_printf(Object(FFI\CData:struct struct2*))
#1 %s(%d): MyClass::do()
#2 {main}
  thrown in %s on line %d
