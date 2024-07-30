--TEST--
FFI/JIT 015: Read Array (nested struct)
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
  $a[0]->a = 0;
  $a[1]->a = 2;
  $a[2]->a = 4;
  $a[3]->a = 6;
  $a[4]->a = 8;
  $x = 0;
  for ($i = 0; $i < 5; $i++) {
  	$x += $a[$i]->a;
  }
  var_dump($x);
}
test();
?>
--EXPECTF--
int(20)
