--TEST--
Weak reference to \FFI\CData
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$cdata_value = \FFI::new('int');
$cdata_array = \FFI::new('int[1]');
$cdata_free = \FFI::new('int[1]', false);
\FFI::free($cdata_free);

$ref_value = \WeakReference::create($cdata_value);
$ref_array = \WeakReference::create($cdata_array);
$ref_free = \WeakReference::create($cdata_free);

var_dump($ref_value->get() === $cdata_value);
var_dump($ref_array->get() === $cdata_array);
var_dump($ref_free->get() === $cdata_free);

unset($cdata_value);
unset($cdata_array);
unset($cdata_free);

var_dump($ref_value->get() === null);
var_dump($ref_array->get() === null);
var_dump($ref_free->get() === null);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
