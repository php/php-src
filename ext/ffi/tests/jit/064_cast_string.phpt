--TEST--
FFI/JIT 064: FFI::cast() (string)
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
  $ffi = FFI::cdef();
  $x = $ffi->new("unsigned int[4]");
  $x[1] = 42;
  for ($i = 0; $i < 5; $i++) {
  	$ret = $ffi->cast("int[4]", $x);
  }
  var_dump($ret);
}
test();
?>
--EXPECTF--
object(FFI\CData:int32_t[4])#%d (4) {
  [0]=>
  int(0)
  [1]=>
  int(42)
  [2]=>
  int(0)
  [3]=>
  int(0)
}
