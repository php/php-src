--TEST--
FFI::free()
--FILE--
<?php

// Ensure there is at least one huge_block
$str = str_repeat('a', 2*1024*1024);

$ffi = FFI::cdef(<<<C
    void *malloc(size_t size);
C);

$ptr = $ffi->malloc(10);
$addr = $ffi->cast("uintptr_t", $ffi->cast("char*", $ptr))->cdata;

$ptr = FFI::cdef()->cast("char*", $addr);

// Should not crash in is_zend_ptr()
FFI::free($ptr);

?>
==DONE==
--EXPECT--
==DONE==
