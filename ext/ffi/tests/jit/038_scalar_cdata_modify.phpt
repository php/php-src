--TEST--
FFI/JIT 038: Scalar CData Modification
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
  $x = FFI::cdef()->new("int");
  for ($i = 0; $i < 5; $i++) {
  	$x->cdata += 42;
  }
  var_dump($x);
}
test();
?>
--EXPECTF--
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(210)
}
