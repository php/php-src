--TEST--
Bug #80847 (Nested structs)
--EXTENSIONS--
ffi
zend_test
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows' && ((1 << 31) > 0)) die('xfail libffi doesn\'t properly support passing big structures by value on Windows/64');
?>
--FILE--
<?php
$header = <<<HEADER
    typedef struct bug80847_01 {
        uint64_t b;
        double c;
    } bug80847_01;

    typedef struct bug80847_02 {
        bug80847_01 a;
    } bug80847_02;

	bug80847_02 ffi_bug80847(bug80847_02 s);
HEADER;

$ffi = FFI::cdef($header);
$x = $ffi->new('bug80847_02');
$x->a->b = 42;
$x->a->c = 42.5;
var_dump($x);
$y = $ffi->ffi_bug80847($x);
var_dump($x, $y);
?>
--EXPECTF--
object(FFI\CData:struct bug80847_02)#%d (1) {
  ["a"]=>
  object(FFI\CData:struct bug80847_01)#%d (2) {
    ["b"]=>
    int(42)
    ["c"]=>
    float(42.5)
  }
}
object(FFI\CData:struct bug80847_02)#%d (1) {
  ["a"]=>
  object(FFI\CData:struct bug80847_01)#%d (2) {
    ["b"]=>
    int(42)
    ["c"]=>
    float(42.5)
  }
}
object(FFI\CData:struct bug80847_02)#%d (1) {
  ["a"]=>
  object(FFI\CData:struct bug80847_01)#%d (2) {
    ["b"]=>
    int(52)
    ["c"]=>
    float(32.5)
  }
}
