--TEST--
GH-14215 (Cannot use FFI::load on CRLF header file with apache2handler)
--EXTENSIONS--
ffi
zend_test
--SKIPIF--
<?php
if(PHP_OS_FAMILY !== "Windows") {
    die('skip only for Windows');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
zend_test_set_fmode(false);
$header_path = __DIR__.'\\gh14215.h';
$ffi = FFI::load($header_path);
var_dump($ffi->GetLastError());
zend_test_set_fmode(true);
?>
--EXPECT--
int(0)
