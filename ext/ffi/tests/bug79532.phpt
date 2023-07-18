--TEST--
Bug #79532 (sizeof off_t can be wrong)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php
require_once('utils.inc');

$header = <<<HEADER
void bug79532(off_t *array, size_t elems);
HEADER;

if (PHP_OS_FAMILY !== 'Windows') {
    $ffi = FFI::cdef($header);
} else {
    try {
        $ffi = FFI::cdef($header, 'php_zend_test.dll');
    } catch (FFI\Exception $ex) {
        $ffi = FFI::cdef($header, ffi_get_php_dll_name());
    }
}

$array = FFI::cdef()->new("off_t[3]");
$ffi->bug79532($array, 3);
var_dump($array);
?>
--EXPECTF--
object(FFI\CData:int%d_t[3])#%d (3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
