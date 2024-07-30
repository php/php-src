--TEST--
FFI/JIT 033: POSTDEC Struct (scalar)
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
  $a = FFI::cdef()->new("struct {int32_t a; int32_t b;}");
  $a->a = 42;
  for ($i = 0; $i < 5; $i++) {
  	$x = $a->a--;
  }
  var_dump($a, $x);
}
test();
?>
--EXPECTF--
object(FFI\CData:struct <anonymous>)#%d (2) {
  ["a"]=>
  int(37)
  ["b"]=>
  int(0)
}
int(38)
