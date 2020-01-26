--TEST--
Bug #78714 (funcs returning pointer can't use call convention spec)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$def = 'char * __cdecl get_zend_version(void);';
if (substr(PHP_OS, 0, 3) != 'WIN') {
    $ffi = FFI::cdef($def);
} else {
    $dll = 'php8' . (PHP_ZTS ? 'ts' : '') . (PHP_DEBUG ? '_debug' : '') . '.dll';
    $ffi = FFI::cdef($def, $dll);
}
echo substr(FFI::string($ffi->get_zend_version()), 0, 4) . "\n";
?>
--EXPECT--
Zend
