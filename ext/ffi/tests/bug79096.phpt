--TEST--
Bug #79096 (FFI Struct Segfault)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php
$header = <<<HEADER
struct bug79096 {
    uint64_t a;
    uint64_t b;
};

struct bug79096 bug79096(void);
HEADER;

$ffi = FFI::cdef($header);
$struct = $ffi->bug79096();
var_dump($struct);
?>
--EXPECTF--
object(FFI\CData:struct bug79096)#%d (2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(1)
}
