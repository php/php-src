--TEST--
FFI/JIT 019: Array Write (nested array)
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
  $a = FFI::cdef()->new("int32_t[5][2]");
  for ($i = 0; $i < 5; $i++) {
  	$a[$i][1] = $i;
  }
  var_dump($a);
}
test();
?>
--EXPECTF--
object(FFI\CData:int32_t[5][2])#%d (5) {
  [0]=>
  object(FFI\CData:int32_t[2])#%d (2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
  }
  [1]=>
  object(FFI\CData:int32_t[2])#%d (2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
  [2]=>
  object(FFI\CData:int32_t[2])#%d (2) {
    [0]=>
    int(0)
    [1]=>
    int(2)
  }
  [3]=>
  object(FFI\CData:int32_t[2])#%d (2) {
    [0]=>
    int(0)
    [1]=>
    int(3)
  }
  [4]=>
  object(FFI\CData:int32_t[2])#%d (2) {
    [0]=>
    int(0)
    [1]=>
    int(4)
  }
}
