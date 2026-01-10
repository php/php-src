--TEST--
Bug #78761 (Zend memory heap corruption with preload and casting)
--EXTENSIONS--
ffi
posix
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (posix_geteuid() == 0) die('skip Cannot run test as root.');
?>
--INI--
opcache.enable_cli=1
opcache.preload={PWD}/bug78761_preload.php
--FILE--
<?php
try {
    FFI::cdef()->cast('char[10]', FFI::cdef()->new('char[1]'));
} catch (FFI\Exception $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
attempt to cast to larger type
