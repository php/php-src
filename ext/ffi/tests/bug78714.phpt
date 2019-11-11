--TEST--
Bug #78714 (funcs returning pointer can't use call convention spec)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef('char * __cdecl get_zend_version(void);');
echo substr(FFI::string($ffi->get_zend_version()), 0, 4) . "\n";
?>
--EXPECT--
Zend
