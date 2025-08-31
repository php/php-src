--TEST--
Weak reference to \FFI
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = \FFI::cdef('');
$ref = \WeakReference::create($ffi);
var_dump($ref->get() === $ffi);
unset($ffi);
var_dump($ref->get() === null);
?>
--EXPECTF--
bool(true)
bool(true)
