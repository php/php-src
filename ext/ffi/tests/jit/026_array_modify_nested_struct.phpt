--TEST--
FFI/JIT 026: Array Modification (nested struct)
--EXTENSIONS--
ffi
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
  $a = FFI::cdef()->new("struct {int32_t a; int32_t b;}[5]");
  for ($i = 0; $i < 5; $i++) {
  	$a[$i]->a += $i;
  }
  var_dump($a);
}
test();
?>
--EXPECTF--
object(FFI\CData:struct <anonymous>[5])#%d (5) {
  [0]=>
  object(FFI\CData:struct <anonymous>)#%d (2) {
    ["a"]=>
    int(0)
    ["b"]=>
    int(0)
  }
  [1]=>
  object(FFI\CData:struct <anonymous>)#%d (2) {
    ["a"]=>
    int(1)
    ["b"]=>
    int(0)
  }
  [2]=>
  object(FFI\CData:struct <anonymous>)#%d (2) {
    ["a"]=>
    int(2)
    ["b"]=>
    int(0)
  }
  [3]=>
  object(FFI\CData:struct <anonymous>)#%d (2) {
    ["a"]=>
    int(3)
    ["b"]=>
    int(0)
  }
  [4]=>
  object(FFI\CData:struct <anonymous>)#%d (2) {
    ["a"]=>
    int(4)
    ["b"]=>
    int(0)
  }
}
