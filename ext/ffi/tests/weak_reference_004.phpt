--TEST--
Using FFI Types for keys of a WeakMap
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$map = new WeakMap();

$ffi = \FFI::cdef('');
$cdata_value = $ffi->new('int');
$cdata_array = $ffi->new('int[1]');
$cdata_free = $ffi->new('int[1]', false);
\FFI::free($cdata_free);
$ctype = $ffi->type('int');

$map[$ffi] = 'ffi';
$map[$cdata_value] = 'cdata_value';
$map[$cdata_array] = 'cdata_array';
$map[$cdata_free] = 'cdata_free';
$map[$ctype] = 'ctype';

var_dump(count($map) === 5);

unset($ffi);
unset($cdata_value);
unset($cdata_array);
unset($cdata_free);
unset($ctype);

var_dump(count($map) === 0);
?>
--EXPECTF--
bool(true)
bool(true)
