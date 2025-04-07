--TEST--
Bug #78714 (funcs returning pointer can't use call convention spec)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$def = 'char * __cdecl get_zend_version(void);';
$ffi = FFI::cdef($def);
echo substr(FFI::string($ffi->get_zend_version()), 0, 4) . "\n";
?>
--EXPECT--
Zend
