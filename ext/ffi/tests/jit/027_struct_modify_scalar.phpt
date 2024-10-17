--TEST--
FFI/JIT 027: Modify Struct (scalar)
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
  $a = FFI::cdef()->new("struct {int32_t a; int32_t b;}");
  for ($i = 0; $i < 5; $i++) {
  	$a->a += $i;
  }
  var_dump($a);
}
test();
?>
--EXPECTF--
object(FFI\CData:struct <anonymous>)#%d (2) {
  ["a"]=>
  int(10)
  ["b"]=>
  int(0)
}
