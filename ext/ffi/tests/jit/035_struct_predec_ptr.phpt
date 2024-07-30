--TEST--
FFI/JIT 035: PREDEC Struct (ptr)
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
  $a = $ffi->new("struct {int32_t *a; int32_t b;}");
  $a->a = $ffi->cast('int32_t*', 42);
  for ($i = 0; $i < 5; $i++) {
  	$x = --$a->a;
  }
  var_dump($ffi->cast('intptr_t', $a->a), $ffi->cast('intptr_t', $x));
}
test();
?>
--EXPECTF--
object(FFI\CData:int%d_t)#%d (1) {
  ["cdata"]=>
  int(22)
}
object(FFI\CData:int%d_t)#%d (1) {
  ["cdata"]=>
  int(22)
}
