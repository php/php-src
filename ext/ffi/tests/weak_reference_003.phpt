--TEST--
Weak reference to \FFI\CType
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ctype = \FFI::type('int');
$ref = \WeakReference::create($ctype);
var_dump($ref->get() === $ctype);
unset($ctype);
var_dump($ref->get() === null);
?>
--EXPECTF--
bool(true)
bool(true)
