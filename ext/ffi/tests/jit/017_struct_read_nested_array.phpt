--TEST--
FFI/JIT 017: Read Struct (nested array)
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
  $a = FFI::cdef()->new("struct {int32_t a[5]; int32_t b;}");
  $a->a[0] = 0;
  $a->a[1] = 2;
  $a->a[2] = 4;
  $a->a[3] = 6;
  $a->a[4] = 8;
  $x = 0;
  for ($i = 0; $i < 5; $i++) {
  	$x += $a->a[$i];
  }
  var_dump($x);
}
test();
?>
--EXPECTF--
int(20)
