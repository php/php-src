--TEST--
Bug #78714 (funcs returning pointer can't use call convention spec)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
require_once('utils.inc');
$def = 'char * __cdecl get_zend_version(void);';
$ffi = ffi_cdef($def, ffi_get_php_dll_name());
echo substr(FFI::string($ffi->get_zend_version()), 0, 4) . "\n";
?>
--EXPECT--
Zend
