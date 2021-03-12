--TEST--
Bug #80847 (Nested structs)
--SKIPIF--
if (!extension_loaded('ffi') die('skip ffi extension not available');
if (!extension_loaded('zend-test') die('skip zend-test extension not available');
--FILE--
<?php
require_once('utils.inc');
$header = <<<CDEF
    typedef struct bug80847_01 {
        uint64_t a;
    } bug80847_01;

    typedef struct bug80847_02 {
        bug80847_01 a;
    } bug80847_02;

    bug80847_02 bug80847(bug80847_02);
CDEF;

if (PHP_OS_FAMILY !== 'Windows') {
    $ffi = FFI::cdef($header);
} else {
    try {
        $ffi = FFI::cdef($header, 'php_zend_test.dll');
    } catch (FFI\Exception $ex) {
        $ffi = FFI::cdef($header, ffi_get_php_dll_name());
    }
}

$test = $ffi->new('bug80847_02');
var_dump($ffi->bug80847($test));
?>
--EXPECT--
object(FFI\CData:struct bug80847_02)#3 (1) {
  ["a"]=>
  object(FFI\CData:struct bug80847_01)#4 (1) {
    ["a"]=>
    int(0)
  }
}
