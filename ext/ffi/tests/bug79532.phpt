--TEST--
Bug #79532 (sizeof off_t can be wrong)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php
$header = <<<HEADER
void bug79532(off_t *array, size_t elems);
HEADER;

$ffi = FFI::cdef($header);
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
