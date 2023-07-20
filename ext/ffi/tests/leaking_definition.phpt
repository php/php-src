--TEST--
FFI::cdef()->new() shouldn't leak definitions into outer scope
--EXTENSIONS--
ffi
--XFAIL--
--FILE--
<?php
$ffi = \FFI::cdef('struct Example { uint32_t x; };');
var_dump($ffi->new('struct Example2 { uint32_t x; }'));
try {
    var_dump($ffi->new('struct Example2'));
} catch (\FFI\ParserException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(FFI\CData:struct Example2)#2 (1) {
  ["x"]=>
  int(0)
}
Incomplete struct "Example2" at line %d
