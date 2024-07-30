--TEST--
FFI/JIT 028: Modify Struct (nested array)
--INI--
ffi.enable=1
;opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=0
opcache.jit_hot_return=0
opcache.jit_hot_side_exit=0
;opcache.jit_debug=0x180005
--FILE--
<?php 
function test() {
  $a = FFI::cdef()->new("struct {int32_t a[5]; int32_t b;}");
  for ($i = 0; $i < 5; $i++) {
  	$a->a[$i] += $i;
  }
  var_dump($a);
}
test();
?>
--EXPECTF--
object(FFI\CData:struct <anonymous>)#%d (2) {
  ["a"]=>
  object(FFI\CData:int32_t[5])#%d (5) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    int(3)
    [4]=>
    int(4)
  }
  ["b"]=>
  int(0)
}
