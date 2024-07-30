--TEST--
FFI/JIT 066: FFI::cast() (scalar)
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
  for ($i = 0; $i < 5; $i++) {
  	$ret = $ffi->cast("double", 42);
  }
  var_dump($ret);
}
test();
?>
--EXPECTF--
object(FFI\CData:double)#%d (1) {
  ["cdata"]=>
  float(42)
}
